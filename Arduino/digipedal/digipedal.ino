//// User-configurable parameters
//   Analogue input pin for pedal potentiometer
constexpr int potPin { A0 };

//   The number of reads averaged to produce the
//   output value (noise reduction measure
constexpr unsigned int readBufSize { 1 };

//   External LED pin
constexpr unsigned int LEDPin { 3 };

//   External LED brightess when nothing happens
constexpr unsigned int dimLED { 5 };

//   External LED brightess when (and shortly after) pedal change
constexpr unsigned int brightLED { 255 };

//   How many times around the main loop before LED goes dim
constexpr unsigned int flashLEDDelay { 800 };

//// A class to preprocess raw pedal vaules before the app sees them.
//   The constructor takes two values:
//     * the number of analog reads to average
//     * the minimum interesting change
//   The first is supposed to get rid of jitter-type noise,
//   and the second stops mechanical vibration causing false
//   triggering. Values of 4 and 3 seem to work OK.
//
//   Calling the object's process(val) method returns a cooked value
//   given a raw one.
class SampleProcessor {
  protected:
    int *readBuf;
    int status { 0 };
    unsigned int readBufHead { 0 };
    const unsigned int readBufLength;
    const unsigned int minChange;
  public:
    SampleProcessor(const unsigned int samples,
                    const unsigned int delta)
      : readBufLength {samples}
      , minChange {delta} {
      readBuf = new int[samples];
    }
    virtual ~SampleProcessor() {
      delete[] readBuf;
    }
    unsigned int process(const int sample) {
      readBuf[readBufHead++] = sample;
      if (readBufHead >= readBufLength)
        readBufHead = 0;
      int sum { 0 };
      for (int i { 0 }; i < readBufLength; i++) {
        sum += readBuf[i];
      }

      int potVal { sum / static_cast<int>(readBufLength) };

      if (abs(potVal - status) >= minChange) {
        status = potVal;
      }
      
      return static_cast<unsigned int>(status);
    }
};

SampleProcessor sp(4, 3);

void setup() {
  Serial.begin(9600);
  analogWrite(LEDPin, dimLED);
}

void loop() {
  static unsigned int result;
  static unsigned int dimLEDCounter;

  // Read a new pedal position value and process it.
  unsigned int newVal { sp.process(analogRead(potPin)) };

  // Do we need to report a change?
  if (newVal != result) {
    // If so, remember the new value,
    result = newVal;
    //   ... make the LED go bright for a bit
    analogWrite(LEDPin, brightLED);
    dimLEDCounter = flashLEDDelay;
    //  ... and send the data normalised in the range 0..1
    //      (four sig fig should be enough)
    Serial.println(result/1023.0, 4);
  }

  // Dim the external LED when it's time.
  if (dimLEDCounter == 1) {
    analogWrite(LEDPin, dimLED);
  }
  if (dimLEDCounter) {
    // Don't decrement past 0!
    --dimLEDCounter;
  }
}
