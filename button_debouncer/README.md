## Team members
Zenovii Popenyuk
Yevhenii Molodtsov

## Description
The principle of operation of the circuit based on the Shmidt trigger an auxiliary circle with a capacitor:
1. When the button is in the off state (the circle is unlocked), the trigger input is given 1 (at output 0), and in parallel, our condenser is recharged with a capacity of 22 microfarads - it has been experimentally established that such a capacity will allow for quenching the clamor we need time.
2. When the button is in the off state (the circle is closed), the capacitor discharges, the trigger stops coming to the input
the signal and its output is changed to 1.
3. The most exciting button state (actually, jerk). Because the capacitor is charged to us, at the moments of clogging (contacting contacts
for a short time), it will compensate for the lack of charge and "shake" the so-called high-frequency noise, supporting the logical unit at the entrance to Shmidt triggers. When the contacts "calm down," then the circuit will go to state 2.
