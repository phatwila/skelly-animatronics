# 12 Foot Skelly Animatronics

3-axis skelly animatronics implemented in easily customizable Arduino code. This project was created for the `3-Axis Skull Mod for 12ft Skeleton` from https://hackaday.io/project/181103-3-axis-skull-mod-for-12ft-skeleton. 

Handles X, Y, Z movement and jaw control. Fully customizable with options for servo range limits, random movements, and speech simulation that can be tuned and tweaked to work with your own DIY project.

## Features

- **3-Axis Head Movement**: Control rotation (left/right), X-axis (side-to-side), and Y-axis (up/down) movements.
- **Jaw Animation**: Synchronized jaw movements for speech simulation.
- **Random Movements**: Adds lifelike random movements between phrases.
- **Breathing Simulation**: Subtle Y-axis movement to simulate breathing.
- **Phrase Generation**: Automatically generates and speaks preset phrases.
- **Customizable Behavior**: Easily adjust movement ranges, speeds, and timings.
- **Modular Design**: Enable or disable specific features as needed.

## Software Dependencies

- Arduino IDE
- Servo library

## Installation

1. Clone this repository:
   ```
   git clone https://github.com/phatwila/skelly-animatronics.git
   ```
2. Open `skelly-animatronics.ino` in the Arduino IDE.
3. Connect your Arduino board to your computer.
4. Select the correct board and port in the Arduino IDE.
5. Upload the sketch to your Arduino board.

## Configuration

You can customize the behavior of the skeleton by modifying the following variables at the top of the `skelly-animatronics.ino` file:

- `ENABLE_JAW_MOVEMENT`: Enable/disable jaw movement
- `ENABLE_HEAD_MOVEMENT`: Enable/disable head movement
- `ENABLE_RANDOM_MOVEMENTS`: Enable/disable random movements
- `ENABLE_BREATHING`: Enable/disable breathing simulation
- `ENABLE_PHRASE_GENERATION`: Enable/disable automatic phrase generation

Additionally, you can fine-tune various aspects of the skeleton's behavior:

- **Servo Ranges**: Adjust `HEAD_ROTATION_MIN/MAX`, `HEAD_X_MIN/MAX`, `HEAD_Y_MIN/MAX`, and `JAW_OPEN_MAX/CLOSED` to set the movement limits for each axis and the jaw.
- **Movement Timing**: Modify `HEAD_MOVE_DURATION`, `RANDOM_MOVE_INTERVAL`, and `BREATH_INTERVAL` to change the frequency of different movements.
- **Movement Speed**: Adjust `HEAD_MOVE_SPEED` and `JAW_SMOOTHING_STEP` to control how quickly the head and jaw move.
- **Speech Timing**: Change `LETTER_DURATION`, `PHRASE_DURATION`, and `JAW_CLOSE_DURATION` to adjust the pacing of speech and pauses.

## Troubleshooting
- ***Ensure servo ranges in the code match your hardware setup.***
- If movements seem erratic, try decreasing the `HEAD_MOVE_SPEED` or increasing `GRADUAL_MOVE_INTERVAL`.

## Contributing

Contributions to improve 12ft Skelly Animatronics are welcome! Please feel free to submit pull requests or open issues to suggest improvements or report bugs. I am specifically interested in enhancing the default settings, as I have not extensively tested them outside of "good enough" for my own projects goals.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by https://hackaday.io/project/181103-3-axis-skull-mod-for-12ft-skeleton

## Author

Ray Detwiler

---

I AM A SKELETON 🎃💀
