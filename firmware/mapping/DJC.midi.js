var DJCDIY = {};

// =======================
// STATE
// =======================
DJCDIY.lastPB = {};
DJCDIY.touchActive = {};
DJCDIY.activeDeck = null;
DJCDIY.wasPlaying = {};

// =======================
// JOG TOUCH (NOTE 0x78)
// NOTE ON  -> value > 0
// NOTE OFF -> value = 0
// =======================
DJCDIY.wheel_touch = function (channel, control, value, status, group) {
    var deck = script.deckFromGroup(group);

    if (value > 0) {
        // -------- TOUCH ON --------
        DJCDIY.activeDeck = deck;
        DJCDIY.touchActive[deck] = true;

        // zapamiętaj czy deck grał
        DJCDIY.wasPlaying[deck] = engine.getValue(group, "play") ? 1 : 0;

        // NATYCHMIAST STOP
        engine.setValue(group, "play", 0);

        // SCRATCH bez inercji
        engine.scratchEnable(
            deck,
            1024,     // platter resolution
            33.333,   // RPM
            3.0,      // alpha (duże tarcie)
            0.0       // beta = 0 (zero inercji)
        );

        DJCDIY.lastPB[deck] = undefined;

    } else {
        // -------- TOUCH OFF --------
        DJCDIY.touchActive[deck] = false;

        // twarde zatrzymanie scratch
        engine.scratchTick(deck, 0);
        engine.scratchTick(deck, 0);

        // wyłącz scratch
        engine.scratchDisable(deck);

        // NATYCHMIAST WRÓĆ PLAY (jeśli wcześniej grał)
        if (DJCDIY.wasPlaying[deck]) {
            engine.setValue(group, "play", 1);
            engine.setValue(group, "play", 1); // wymuszenie
        }

        DJCDIY.lastPB[deck] = undefined;
        DJCDIY.activeDeck = null;
    }
};

// =======================
// JOG ROTATION (PITCH BEND)
// status: 0xE0
// control = LSB, value = MSB
// =======================
DJCDIY.pitchbend = function (channel, control, value, status, group) {

    var deck = DJCDIY.activeDeck;
    if (!deck) return;
    if (!DJCDIY.touchActive[deck]) return;

    // ----- 14-bit Pitch Bend -----
    var pb14 = (value << 7) | control; // MSB<<7 | LSB

    // pierwszy tick = NATYCHMIASTOWY START
    if (DJCDIY.lastPB[deck] === undefined) {
        DJCDIY.lastPB[deck] = pb14;
        engine.scratchTick(deck, 0.0001); // „kick” silnika
        return;
    }

    var movement = pb14 - DJCDIY.lastPB[deck];
    DJCDIY.lastPB[deck] = pb14;

    // wrap 14-bit
    if (movement > 8192)  movement -= 16384;
    if (movement < -8192) movement += 16384;

    // DEADZONE = zero drgań
    var DEADZONE = 1;

    if (Math.abs(movement) <= DEADZONE) {
        engine.scratchTick(deck, 0);
        engine.scratchTick(deck, 0);
        return;
    }

    // czułość scratcha
    var SENSITIVITY = 0.08;

    engine.scratchTick(deck, movement * SENSITIVITY);
};

// =======================
// TEMPO NUDGE (CC 0x2A)
// =======================
// =======================
// TEMPO NUDGE (CC 0x2A)
// =======================
DJCDIY.tempoNudge = function (channel, control, value, status, group) {

    // środek = nic
    if (value === 0x40) return;

    var direction;
    if (value > 0x40) {
        direction = "rate_temp_up";
    } else {
        direction = "rate_temp_down";
    }

    // WCIŚNIJ
    engine.setValue(group, direction, 1);

    // PUŚĆ po 25 ms
    engine.beginTimer(40, function () {
        engine.setValue(group, direction, 0);
    }, true);
};


// =======================
// LOOP SIZE ENCODER
// =======================

// dostępne rozmiary loopa (jak w UI Mixxx)
DJCDIY.loopSizes = [0.125, 0.25, 0.5, 1, 2, 4, 8, 16];
DJCDIY.loopIndex = {
    1: 3, // start = 1 beat
    2: 3
};


// LOOP SIZE CHANGE
// =======================
// LOOP SIZE CONTROL (NOTE +/-)
// =======================

// Dostępne rozmiary loopa (jak w Mixxx)
DJCDIY.loopSizes = [0.125, 0.25, 0.5, 1, 2, 4, 8, 16];

// Startowy index = 1 beat
DJCDIY.loopIndex = {
    1: 3,
    2: 3
};

// PLUS
DJCDIY.loopSizePlus = function (channel, control, value, status, group) {
    if (value === 0) return; // NOTE OFF ignorujemy

    const deck = script.deckFromGroup(group);

    DJCDIY.loopIndex[deck] = Math.min(
        DJCDIY.loopIndex[deck] + 1,
        DJCDIY.loopSizes.length - 1
    );

    engine.setValue(
        group,
        "beatloop_size",
        DJCDIY.loopSizes[DJCDIY.loopIndex[deck]]
    );
};

// MINUS
DJCDIY.loopSizeMinus = function (channel, control, value, status, group) {
    if (value === 0) return; // NOTE OFF ignorujemy

    const deck = script.deckFromGroup(group);

    DJCDIY.loopIndex[deck] = Math.max(
        DJCDIY.loopIndex[deck] - 1,
        0
    );

    engine.setValue(
        group,
        "beatloop_size",
        DJCDIY.loopSizes[DJCDIY.loopIndex[deck]]
    );
};


// =====================================================
// MODE 3 – LOOP / REMIX (GEST + TAP IMPULSE SAFE)
// =====================================================

// --- CC1: LOOP SIZE ---
DJCDIY.mode3_loopSize = function (channel, control, value, status, group) {
    var deck = script.deckFromGroup(group);
    var index = Math.floor(value / 16); // 0–7

    DJCDIY.loopIndex[deck] = Math.max(
        0,
        Math.min(index, DJCDIY.loopSizes.length - 1)
    );

    engine.setValue(
        group,
        "beatloop_size",
        DJCDIY.loopSizes[DJCDIY.loopIndex[deck]]
    );
};

// --- CC2: LOOP MOVE ---
DJCDIY.mode3_loopMove = function (channel, control, value, status, group) {
    if (!engine.getValue(group, "beatloop_activate")) return;
    engine.setValue(group, "loop_move", (value - 64) / 64);
};

// --- CC3: LOOP GAIN (SUBTLE) ---
DJCDIY.mode3_loopGain = function (channel, control, value, status, group) {
    engine.setValue(group, "pregain", 0.3 + (value / 127) * 0.9);
};

// --- TAP1: LOOP TOGGLE ---
DJCDIY.mode3_loopToggle = function (channel, control, value, status, group) {
    if (value === 0) return;

    engine.setValue(
        group,
        "beatloop_activate",
        engine.getValue(group, "beatloop_activate") ? 0 : 1
    );
};

// --- TAP2: RELOOP ---
DJCDIY.mode3_reloop = function (channel, control, value, status, group) {
    if (value === 0) return;
    engine.setValue(group, "reloop_toggle", 1);
};

// --- TAP3: LOOP ROLL (AUTO-OFF) ---
DJCDIY.mode3_loopRoll = function (channel, control, value, status, group) {
    if (value === 0) return;

    engine.setValue(group, "beatlooproll_activate", 1);

    // auto-off (tap-safe)
    engine.beginTimer(150, function () {
        engine.setValue(group, "beatlooproll_activate", 0);
    }, true);
};

DJCDIY.getQuickFxGroup = function (group) {
    var deck = script.deckFromGroup(group);
    return "[QuickEffectRack1_[Channel" + deck + "]]";
};
// =====================================================
// MODE 4 – BUILD / DROP (FULL GESTURE CONTROL)
// =====================================================

// --- CC1: FILTER SWEEP ---
DJCDIY.mode4_filter = function (channel, control, value, status, group) {
    engine.setValue(
        DJCDIY.getQuickFxGroup(group),
        "super1",
        value / 127
    );
};

// --- CC2: BUILD MACRO (FILTER + FULL DUCK via PREGain) ---
DJCDIY.mode4_buildMacro = function (channel, control, value, status, group) {
    var n = value / 127;

    // filter closes
    engine.setValue(
        DJCDIY.getQuickFxGroup(group),
        "super1",
        0.5 - n * 0.5
    );

    // FULL duck (0–1, real silence)
    engine.setValue(group, "pregain", 1 - n);
};

// --- CC3: ENERGY RETURN (PUNCH BACK) ---
DJCDIY.mode4_energy = function (channel, control, value, status, group) {
    engine.setValue(group, "pregain", value / 127);
};

// --- TAP1: INSTANT BUILD ---
DJCDIY.mode4_build = function (channel, control, value, status, group) {
    if (value === 0) return;

    engine.setValue(DJCDIY.getQuickFxGroup(group), "super1", 0.5);
    engine.setValue(group, "pregain", 0.1);
};

// --- TAP2: DROP (RESET EVERYTHING) ---
DJCDIY.mode4_drop = function (channel, control, value, status, group) {
    if (value === 0) return;

    engine.setValue(DJCDIY.getQuickFxGroup(group), "super1", 0.5);
    engine.setValue(group, "pregain", 1);
};

// --- TAP3: ACCENT HIT (SHORT MUTE) ---
DJCDIY.mode4_accent = function (channel, control, value, status, group) {
    if (value === 0) return;

    engine.setValue(group, "pregain", 0);

    engine.beginTimer(80, function () {
        engine.setValue(group, "pregain", 1);
    }, true);
};
