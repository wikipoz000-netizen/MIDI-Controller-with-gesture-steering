#include "Gestures.h"
#include "Config.h"
#include "State.h"
#include "MIDI.h"
#include <Arduino.h>

/* =====================================================
   ================= DEBUG =============================
   ===================================================== */

#define GESTURE_DEBUG 1

#if GESTURE_DEBUG
  #define DBG(x) Serial.println(x)
  #define DBG2(x,y) { Serial.print(x); Serial.println(y); }
#else
  #define DBG(x)
  #define DBG2(x,y)
#endif

// ===== DEBUG EXPORTS =====
int dbgDist[3]    = { -1, -1, -1 };
bool dbgActive[3] = { false, false, false };

/* =====================================================
   ================= HC-SR04 PINS ======================
   ===================================================== */

#define TRIG_PIN     16
#define ECHO_LEFT    19
#define ECHO_CENTER  17
#define ECHO_RIGHT   18

static const uint8_t echoPins[3] = {
  ECHO_LEFT,
  ECHO_CENTER,
  ECHO_RIGHT
};

/* =====================================================
   ================= PARAMS ============================
   ===================================================== */

#define GESTURE_MIN_CM     2
#define GESTURE_MAX_CM    20

#define TAP_MIN_HOLD_MS   50
#define TAP_MAX_HOLD_MS   250
#define TAP_MIN_DELTA_CM   6
#define TAP_MAX_DIST_CM    6

#define SWIPE_TIME_MS    250
 
/* =====================================================
   ================= SENSOR STATE ======================
   ===================================================== */

struct SensorState {
  bool active;
  bool ccActive;          // CC tylko po długim trzymaniu
  uint32_t onTime;
  uint32_t offTime;
  int minDist;
  int maxDist;
};

static SensorState sensors[3];

static int lastSensorSeen = -1;
static uint32_t lastSensorSeenTime = 0;

/* =====================================================
   ================= MIDI MAP ==========================
   ===================================================== */

struct GestureMidi {
  MidiType type;
  uint8_t number;
};

// [deck][mode][sensor][gesture]
GestureMidi gestureMap[2][4][3][6];

/* =====================================================
   ================= MIDI MAP INIT =====================
   ===================================================== */

void initGestureMap() {

  memset(gestureMap, 0, sizeof(gestureMap));

  // ===== CC UP/DOWN =====
  static const uint8_t CC_UP_A[4][3] = {
    {48, 49, 50},   // Mode 1
    {51, 52, 53},   // Mode 2
    {54, 55, 56},   // Mode 3
    {57, 58, 59}    // Mode 4
  };

  static const uint8_t CC_UP_B[4][3] = {
    {80, 81, 82},   // Mode 1
    {83, 84, 85},   // Mode 2
    {86, 87, 88},   // Mode 3
    {89, 90, 91}    // Mode 4
  };

  // ===== NOTE TAP =====
  static const uint8_t TAP_A[4][3] = {
    {96,  97,  98},   // Mode 1
    {99, 100, 101},   // Mode 2
    {102,103,104},    // Mode 3
    {105,106,107}     // Mode 4
  };

  static const uint8_t TAP_B[4][3] = {
    {108,109,110},    // Mode 1
    {111,112,113},    // Mode 2
    {114,115,116},    // Mode 3
    {117,118,119}     // Mode 4
  };

  // ---------- MAPOWANIE ----------
  for (uint8_t mode = 0; mode < 4; mode++) {
    for (uint8_t s = 0; s < 3; s++) {

      // --- UP / DOWN ---
      gestureMap[DECK_A][mode][s][G_UPDOWN] = {
        MIDI_CC, CC_UP_A[mode][s]
      };

      gestureMap[DECK_B][mode][s][G_UPDOWN] = {
        MIDI_CC, CC_UP_B[mode][s]
      };

      // --- TAP ---
      gestureMap[DECK_A][mode][s][G_TAP] = {
        MIDI_NOTE, TAP_A[mode][s]
      };

      gestureMap[DECK_B][mode][s][G_TAP] = {
        MIDI_NOTE, TAP_B[mode][s]
      };
    }
  }
}




/* =====================================================
   ================= HC-SR04 READ ======================
   ===================================================== */

int readDistance(uint8_t echoPin) {
  long t = pulseIn(echoPin, HIGH, 25000);
  if (t == 0) return -1;

  int cm = t / 58;
  if (cm < GESTURE_MIN_CM || cm > GESTURE_MAX_CM) return -1;
  return cm;
}

/* =====================================================
   ================= INIT ==============================
   ===================================================== */

void initGestures() {

  pinMode(TRIG_PIN, OUTPUT);

  for (uint8_t i = 0; i < 3; i++) {
    pinMode(echoPins[i], INPUT);
    sensors[i] = { false, false, 0, 0, 0, 0 };
  }

  initGestureMap();

#if GESTURE_DEBUG
  Serial.println("GESTURE SYSTEM READY");
#endif
}

/* =====================================================
   ================= UPDATE ============================
   ===================================================== */

void updateGestures() {

  static uint8_t trigIndex = 0;
  static uint32_t lastTrigTime = 0;

  const uint32_t TRIG_INTERVAL_MS = 30;
  uint32_t now = millis();

  if (now - lastTrigTime < TRIG_INTERVAL_MS) return;
  lastTrigTime = now;

  uint8_t i = trigIndex;
  trigIndex = (trigIndex + 1) % 3;

  // ---------- TRIGGER ----------
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  int d = readDistance(echoPins[i]);
  bool seen = (d > 0);

  /* ---------- HAND ON ---------- */
  if (seen && !sensors[i].active) {
    sensors[i].active   = true;
    sensors[i].ccActive = false;
    sensors[i].onTime   = now;
    sensors[i].minDist  = d;
    sensors[i].maxDist  = d;

    dbgActive[i] = true;

    sendGesture(i, G_HAND_ON, 127);

    lastSensorSeen = i;
    lastSensorSeenTime = now;
  }

  /* ---------- UP / DOWN (CC PO CZASIE) ---------- */
  if (seen && sensors[i].active) {

    uint32_t holdTime = now - sensors[i].onTime;

    if (!sensors[i].ccActive && holdTime > TAP_MAX_HOLD_MS) {
      sensors[i].ccActive = true;
    }

    if (sensors[i].ccActive) {
      sensors[i].minDist = min(sensors[i].minDist, d);
      sensors[i].maxDist = max(sensors[i].maxDist, d);

      uint8_t ccVal = map(d, GESTURE_MIN_CM, GESTURE_MAX_CM, 0, 127);
      ccVal = constrain(ccVal, 0, 127);

      dbgDist[i] = ccVal;
      sendGesture(i, G_UPDOWN, ccVal);
    }
  }

  /* ---------- HAND OFF + TAP ---------- */
  if (!seen && sensors[i].active) {

    sensors[i].active   = false;
    sensors[i].ccActive = false;
    sensors[i].offTime  = now;

    dbgActive[i] = false;
    dbgDist[i]   = -1;

    sendGesture(i, G_HAND_OFF, 0);

    uint32_t holdTime = sensors[i].offTime - sensors[i].onTime;
    int deltaDist = sensors[i].maxDist - sensors[i].minDist;

    if (holdTime >= TAP_MIN_HOLD_MS &&
        holdTime <= TAP_MAX_HOLD_MS &&
        deltaDist <= TAP_MIN_DELTA_CM &&
        sensors[i].minDist <= TAP_MAX_DIST_CM) {

      DBG2("TAP S", i);
      sendGesture(i, G_TAP, 127);
    }
  }

  /* ---------- GLOBAL SWIPE ---------- */
  if (lastSensorSeen >= 0) {
    for (uint8_t s = 0; s < 3; s++) {
      if (sensors[s].active && s != lastSensorSeen) {
        if (now - lastSensorSeenTime < SWIPE_TIME_MS) {

          if (
            (lastSensorSeen == 0 && s == 1) ||
            (lastSensorSeen == 1 && s == 2) ||
            (lastSensorSeen == 0 && s == 2)
          ) {
            DBG("SWIPE L -> R");
            sendGesture(1, G_SWIPE_R, 127);
          }

          if (
            (lastSensorSeen == 2 && s == 1) ||
            (lastSensorSeen == 1 && s == 0) ||
            (lastSensorSeen == 2 && s == 0)
          ) {
            DBG("SWIPE R -> L");
            sendGesture(1, G_SWIPE_L, 127);
          }
        }
        lastSensorSeen = -1;
        break;
      }
    }
  }
}

/* =====================================================
   ================= SEND MIDI =========================
   ===================================================== */

void sendGesture(uint8_t sensor, GestureType gesture, uint8_t value) {

  GestureMode mode = activeGestureMode;

  // MODE_NONE blokuje tylko gesty "funkcyjne"
  if (mode == MODE_NONE) {
    if (gesture != G_HAND_ON && gesture != G_HAND_OFF)
      return;
  }

  Deck deck = activeDeck;
  GestureMidi gm = gestureMap[deck][mode][sensor][gesture];

  if (gm.number == 0) return;

  uint8_t ch = (deck == DECK_A) ? MIDI_CH_A : MIDI_CH_B;

  if (gm.type == MIDI_CC) {
    sendCC(ch, gm.number, value);
  }
  else if (gm.type == MIDI_NOTE) {
    sendNote(ch, gm.number, value);
  }
}
