#       _______
#    .-'       '-.
#   /             \
#  |               |
#  |,  .-. .-.  ,  |
#  | )(__/ \__)(   |
#  |/     /\     \ |
#  (_     ^^     _)
#   \__|IIIIII|__/
#    | \IIIIII/ |
#    \          /
#     `--------`
#  MIT License 2024
#  Author: Ray Detwiler

#include <Servo.h>

// FEATURE CONFIGURATION
// Set these to true or false to enable or disable specific features
const bool ENABLE_JAW_MOVEMENT = true;     // Enable/disable jaw movement for speech
const bool ENABLE_HEAD_MOVEMENT = true;    // Enable/disable major head movements
const bool ENABLE_RANDOM_MOVEMENTS = true; // Enable/disable small random movements between phrases
const bool ENABLE_BREATHING = true;        // Enable/disable subtle breathing motion
const bool ENABLE_PHRASE_GENERATION = true; // Enable/disable automatic phrase generation

// ADJUSTABLE VARIABLES
// Modify these to fine-tune the skeleton's behavior

// Servo pin assignments
const int SKULL_R_PIN = 8;   // Rotation servo pin (left to right)
const int SKULL_X_PIN = 9;   // X-axis servo pin (side to side)
const int SKULL_Y_PIN = 10;  // Y-axis servo pin (up and down)
const int SKULL_JAW_PIN = 11; // Jaw servo pin

// Servo position ranges
// Adjusting these will change the range of motion for each axis
const int DEFAULT_POSITION = 90;  // Neutral position for all servos
const int JAW_CLOSED = 115;       // Position for a closed jaw (higher number = more closed)
const int JAW_OPEN_MAX = 70;      // Maximum open position for the jaw (lower number = more open)
const int HEAD_ROTATION_MIN = 60; // Minimum rotation angle (further left)
const int HEAD_ROTATION_MAX = 120; // Maximum rotation angle (further right)
const int HEAD_X_MIN = 75;        // Minimum X-axis angle (further left)
const int HEAD_X_MAX = 115;       // Maximum X-axis angle (further right)
const int HEAD_Y_MIN = 75;        // Minimum Y-axis angle (further down)
const int HEAD_Y_MAX = 115;       // Maximum Y-axis angle (further up)

// Timing variables (in milliseconds)
// Adjusting these will change the speed and frequency of movements
const unsigned long LETTER_DURATION = 100;    // Duration for each letter when speaking (lower = faster speech)
const unsigned long HEAD_MOVE_DURATION = 5000; // Time between major head movements (higher = less frequent big moves)
const unsigned long GRADUAL_MOVE_INTERVAL = 20; // Interval for updating servo positions (lower = smoother but more CPU intensive)
const unsigned long JAW_SMOOTH_INTERVAL = 10;   // Interval between jaw smoothing steps (lower = smoother jaw movement)
const unsigned long RANDOM_MOVE_INTERVAL = 1000; // Interval between small random movements (lower = more frequent small moves)
const unsigned long PHRASE_DURATION = 10000;    // Time between phrases (higher = longer pauses between speaking)
const unsigned long JAW_CLOSE_DURATION = 200;   // Time to keep jaw closed between words (higher = longer pauses in speech)

// Movement parameters
const int JAW_SMOOTHING_STEP = 5;  // Size of each jaw movement step (higher = faster but less smooth jaw movement)
const int RANDOM_MOVE_RANGE = 35;  // Range of random movements (higher = more extreme random movements)
const float HEAD_MOVE_SPEED = 8.0; // Speed of head movements in degrees per second (higher = faster head movements)

// Breathing simulation parameters
const unsigned long BREATH_INTERVAL = 3000; // Duration of one breath cycle in milliseconds (higher = slower breathing)
const float BREATH_AMPLITUDE = 2.0;         // Maximum movement caused by breathing in degrees (higher = more noticeable breathing)

// Create Servo objects
Servo skull_r_servo;  // Controls the rotation (left to right)
Servo skull_x_servo;  // Controls the X-axis (side to side)
Servo skull_y_servo;  // Controls the Y-axis (up and down)
Servo skull_jaw_servo; // Controls the jaw movement (open and close)

// Timing variables for managing various movement and behavior cycles
unsigned long lastLetterTime = 0;    // Tracks the last time a letter was processed for speech
unsigned long lastHeadMoveTime = 0;  // Records the last time a major head movement was initiated
unsigned long lastGradualMoveTime = 0; // Keeps track of the last gradual servo movement update
unsigned long lastJawSmoothTime = 0;   // Monitors the last jaw position update for smooth movement
unsigned long lastRandomMoveTime = 0;  // Tracks the last time a random movement was generated
unsigned long lastPhraseTime = 0;      // Records the last time a new phrase was started
unsigned long lastBreathTime = 0;      // Keeps track of the last breathing cycle update

// These variables work with millis() to create non-blocking delays.
// Example usage:
// if (millis() - lastHeadMoveTime >= HEAD_MOVE_DURATION) {
//   // Perform head movement
//   // ...
//   lastHeadMoveTime = millis();  // Reset the timer
// }

// Position variables
float targetRotation = DEFAULT_POSITION;
float targetX = DEFAULT_POSITION;
float targetY = DEFAULT_POSITION;
float currentRotation = DEFAULT_POSITION;
float currentX = DEFAULT_POSITION;
float currentY = DEFAULT_POSITION;
float breathOffset = 0;

// Speech variables
char currentPhrase[20] = ""; // Adjust size based on your longest phrase
int currentLetterIndex = 0;

// Mapping letters to jaw positions for expressive speech synchronization
const int letterToJaw[] = {
  50, 25, 38, 25, 50, // A, B, C, D, E
  25, 38, 25, 50, 25, // F, G, H, I, J
  25, 25, 38, 38, 50, // K, L, M, N, O
  25, 25, 38, 38, 25, // P, Q, R, S, T
  50, 25, 25, 38, 50, // U, V, W, X, Y
  25                  // Z
};

// Array of preset phrases 
const char* phrases[] = {
  "HELLO THERE",
  "HOW ARE YOU",
  "I AM A SKELETON",
  "NICE TO MEET YOU",
  "WELCOME TO MY LAIR",
  "BEWARE OF GHOSTS",
  "HAPPY HALLOWEEN",
  "BOO DID I SCARE YOU",
  "LETS HAVE SOME FUN",
  "TIME FOR A PARTY"
};
const int numPhrases = sizeof(phrases) / sizeof(phrases[0]);

// Setup function: Initializes the skeleton's servos and sets up initial conditions
void setup() {
  // Initialize random seed for better randomness
  randomSeed(analogRead(0) + millis());

  // Attach servos to their respective pins
  skull_r_servo.attach(SKULL_R_PIN);
  skull_x_servo.attach(SKULL_X_PIN);
  skull_y_servo.attach(SKULL_Y_PIN);
  skull_jaw_servo.attach(SKULL_JAW_PIN);

  // Set all servos to their default positions
  skull_r_servo.write(DEFAULT_POSITION);
  skull_x_servo.write(DEFAULT_POSITION);
  skull_y_servo.write(DEFAULT_POSITION);
  skull_jaw_servo.write(JAW_CLOSED);

  Serial.begin(9600);
}

// Main loop: Continuously updates the skeleton's movements and behaviors
void loop() {
  unsigned long currentTime = millis();

  // Execute enabled features
  if (ENABLE_JAW_MOVEMENT) {
    updateJawMovement();    // Handle jaw movements for synchronized speech
  }
  if (ENABLE_HEAD_MOVEMENT) {
    updateHeadMovement();   // Update head position for dynamic movements
  }
  if (ENABLE_RANDOM_MOVEMENTS) {
    randomMovementsBetweenSentences(); // Add random movements between sentences
  }
  if (ENABLE_BREATHING) {
    updateBreathing(currentTime);      // Apply subtle breathing movement
  }
  graduallyMoveServos();  // Smoothly transition servos to target positions
  if (ENABLE_PHRASE_GENERATION) {
    generateNewPhrase();    // Generate new phrases to keep the interaction lively
  }
}

// Function to manage jaw movements for speech synchronization
void updateJawMovement() {
  unsigned long currentTime = millis();

  if (currentPhrase[0] != '\0') {
    if (currentTime - lastLetterTime >= LETTER_DURATION) {
      char currentLetter = currentPhrase[currentLetterIndex];
      int targetJawPosition = JAW_CLOSED;

      if (isSilentCharacter(currentLetter)) {
        targetJawPosition = JAW_CLOSED;
        delay(JAW_CLOSE_DURATION);
      } else {
        int jawPosition = getJawPositionForLetter(currentLetter);
        targetJawPosition = map(jawPosition, 0, 50, JAW_CLOSED, JAW_OPEN_MAX);
      }

      smoothJawMovement(targetJawPosition);
      currentLetterIndex++;
      
      if (currentLetterIndex >= strlen(currentPhrase)) {
        currentPhrase[0] = '\0';
        currentLetterIndex = 0;
        smoothJawMovement(JAW_CLOSED); // Ensure jaw closes smoothly at the end
      }

      lastLetterTime = currentTime;
    }
  } else {
    smoothJawMovement(JAW_CLOSED);
  }
}

// Function to add dynamic head movements
void updateHeadMovement() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastHeadMoveTime >= HEAD_MOVE_DURATION) {
    // Generate random expressive head positions
    targetRotation = random(HEAD_ROTATION_MIN, HEAD_ROTATION_MAX + 1);
    targetX = random(HEAD_X_MIN, HEAD_X_MAX + 1);
    targetY = random(HEAD_Y_MIN, HEAD_Y_MAX + 1);
    lastHeadMoveTime = currentTime;
    lastRandomMoveTime = currentTime; // Reset random move timer to avoid immediate random movement
  }
}

// Function to add random movements between sentences
void randomMovementsBetweenSentences() {
  unsigned long currentTime = millis();
  
  if (currentPhrase[0] == '\0' && currentTime - lastRandomMoveTime >= RANDOM_MOVE_INTERVAL) {
    // Generate small random adjustments for head servos
    float randomR = random(-RANDOM_MOVE_RANGE, RANDOM_MOVE_RANGE + 1) / 10.0;
    float randomX = random(-RANDOM_MOVE_RANGE, RANDOM_MOVE_RANGE + 1) / 10.0;
    float randomY = random(-RANDOM_MOVE_RANGE, RANDOM_MOVE_RANGE + 1) / 10.0;

    // Apply random adjustments to target positions
    targetRotation = constrain(targetRotation + randomR, HEAD_ROTATION_MIN, HEAD_ROTATION_MAX);
    targetX = constrain(targetX + randomX, HEAD_X_MIN, HEAD_X_MAX);
    targetY = constrain(targetY + randomY, HEAD_Y_MIN, HEAD_Y_MAX);

    lastRandomMoveTime = currentTime;
  }
}

// Function to apply subtle breathing movement
void updateBreathing(unsigned long currentTime) {
  // Update every 20ms for smooth movement (50Hz update rate)
  if (currentTime - lastBreathTime >= 20) {
    float breathCycle = (currentTime % BREATH_INTERVAL) / (float)BREATH_INTERVAL;
    breathOffset = sin(breathCycle * 2 * PI) * BREATH_AMPLITUDE;
    targetY += breathOffset;
    targetY = constrain(targetY, HEAD_Y_MIN, HEAD_Y_MAX);
    lastBreathTime = currentTime;
  }
}

// Function to gradually transition head servos to their target positions
void graduallyMoveServos() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastGradualMoveTime >= GRADUAL_MOVE_INTERVAL) {
    float elapsedSeconds = (currentTime - lastGradualMoveTime) / 1000.0;
    float maxMove = HEAD_MOVE_SPEED * elapsedSeconds;

    // Gradual rotation movement
    float newRotation = moveTowards(currentRotation, targetRotation, maxMove);
    if (newRotation != currentRotation) {
      currentRotation = newRotation;
      skull_r_servo.write(round(currentRotation));
    }

    // Gradual X-axis movement
    float newX = moveTowards(currentX, targetX, maxMove);
    if (newX != currentX) {
      currentX = newX;
      skull_x_servo.write(round(currentX));
    }

    // Gradual Y-axis movement
    float newY = moveTowards(currentY, targetY, maxMove);
    if (newY != currentY) {
      currentY = newY;
      skull_y_servo.write(round(currentY));
    }

    lastGradualMoveTime = currentTime;
  }
}

// Function to handle new phrase generation for the skeleton
void generateNewPhrase() {
  unsigned long currentTime = millis();
  
  if (currentPhrase[0] == '\0' && currentTime - lastPhraseTime >= PHRASE_DURATION) {
    int phraseIndex = random(numPhrases);
    strncpy(currentPhrase, phrases[phraseIndex], sizeof(currentPhrase) - 1);
    currentPhrase[sizeof(currentPhrase) - 1] = '\0'; // Ensure null-termination
    currentLetterIndex = 0;
    lastPhraseTime = currentTime;
    
    Serial.print("New phrase: ");
    Serial.println(currentPhrase);
  }
}

// Function to determine the jaw position based on the spoken letter
int getJawPositionForLetter(char letter) {
  if (letter >= 'A' && letter <= 'Z') {
    return letterToJaw[letter - 'A'];
  }
  return 0; // Default to closed position for non-alphabetic characters
}

// Function to smoothly transition the jaw to a target position
void smoothJawMovement(int targetPosition) {
  unsigned long currentTime = millis();
  int currentPosition = skull_jaw_servo.read();

  if (abs(currentPosition - targetPosition) > JAW_SMOOTHING_STEP * 2) {
    skull_jaw_servo.write(targetPosition);
  } else if (currentTime - lastJawSmoothTime >= JAW_SMOOTH_INTERVAL) {
    if (currentPosition < targetPosition) {
      currentPosition += JAW_SMOOTHING_STEP;
      if (currentPosition > targetPosition) currentPosition = targetPosition;
    } else if (currentPosition > targetPosition) {
      currentPosition -= JAW_SMOOTHING_STEP;
      if (currentPosition < targetPosition) currentPosition = targetPosition;
    }
    skull_jaw_servo.write(currentPosition);
    lastJawSmoothTime = currentTime;
  }
}

// Helper function to move a value towards a target at a given rate
// This function is used for smooth transitions in servo movements
float moveTowards(float current, float target, float maxMove) {
  if (abs(current - target) <= maxMove) {
    return target;
  } else if (current < target) {
    return current + maxMove;
  } else {
    return current - maxMove;
  }
}

// Helper function to determine if a character is a silent one (e.g., space, period, comma, etc.)
bool isSilentCharacter(char c) {
  return (c == ' ' || c == '.' || c == ',' || c == '!');
}