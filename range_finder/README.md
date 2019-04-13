## Team members
Zenovii Popenyuk
Yevhenii Molodtsov

## Description
Extends 16 ms on the pin TRIG 1, then changes to 0. Then the ECHO value automatically adjusts to the foam, because of which
the interrupt is called and if the state is the expectation of sending the ECHO signal, then the current time is recorded.
If the state is the state of waiting for the signal, then the difference between the sending time and the signal is recorded

If the distance is less than 5 cm or more than 5 meters, then an error is shown since the rangefinder does not detect a range less than 5 cm, and the distance more than 5 meters he counts wrongly.
If the signal was not sent, or sent, but not "come back" then the program freezes.
