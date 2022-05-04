# Calibration

## Plan

1. With the circuit constructed as described above and the code tagged as `0.1` collect temperature vs. ADC couint readings.
1. Develop a relationship between couints and temperature that could be used to calculate temperature form the counts.

## Procedure

The MQTT messages produced by trhis code (when captured using `mosquitto_sub -v -t \#`) look like

```text
esp32.1/pub uptime 60, timestamp 1651598127, ch1 3007, ch2 3160
esp32.1/pub uptime 80, timestamp 1651598167, ch1 3011, ch2 3160
esp32.1/pub uptime 420, timestamp 1651598847, ch1 2952, ch2 3160
esp32.1/pub uptime 700, timestamp 1651599407, ch1 2870, ch2 3160
```

To capture the readings and temperature I took a second probe connected to the Maverick remote thermometer, wrapped the tips together with foil and then bound them together with aluminum wire. The intent was to have them both see the same temperature. (A good scientist would verify this. I am not that good.)

I then ran the code with the probes in an oven resting on a cast iron skillet and raised the temperature in 25° (and as temperatures got higher, 50°F) steps. While this was running, I ran the `mosquitto_sub` command, redirecting output to a text file and then opened the text file with VS Code (which I have set to autosave.) When a reading came in from the MQTT cliewnt, I could add the indicated temperature to the end of the line and VS Code would save that before the next line came in. If I took too long, VS Code wouold advise me that the file had changed on disk and I would force write it and lose a report from the MQTT client. The results then looked like

```text
esp32.1/pub uptime 60, timestamp 1651598127, ch1 3007, ch2 3160, 104
esp32.1/pub uptime 80, timestamp 1651598167, ch1 3011, ch2 3160, 104
esp32.1/pub uptime 420, timestamp 1651598847, ch1 2952, ch2 3160, 113
esp32.1/pub uptime 700, timestamp 1651599407, ch1 2870, ch2 3160, 122
esp32.1/pub uptime 720, timestamp 1651599447, ch1 2865, ch2 3160, 124
```

Later I felt I had insufficient data for temperatures below boiling so I repeated with the probes in a pot of water that was slowly brought to a boil.


The complete files are in this directory as `2022-05-03-readings.txt` and `2022-05-01-readings.txt`.

I then used `awk` to isolate the reading and temperature intro a corresponding .csv file.

```text
hbarta@olive:~/Programming/ESP32/BBQ-temp-publisher/calibration$ head -3 2022-05-03-readings.txt | awk '{ print $7 $10 }'
3007,104
3011,104
2952,113
hbarta@olive:~/Programming/ESP32/BBQ-temp-publisher/calibration$
```

The .csv files were concatenated and fed into the Python scripts which calculated the polynomials to be used to caldulate temperature.

## 2022-05-02 curve fitting

* <https://machinelearningmastery.com/curve-fitting-with-python/>

Start with `polynomial regression model by adding squared terms to the objective function.` See `second-deg-poly.md`

```text
apt install python3-numpy
apt install python3-pandas
apt install python3-scipy
apt install python3-matplotlib
```

Ran examples on the second through fifth order polynomials. Fifth looks best, no surprise. There is a gap in the lower ranges which necessitated the second data collection.

Collected data using a pot of water warmed from "hot tap" through boiling. Concatenated that with the other data and reran the curves. Fifth order still looks really good. The question is if this will result in overflow in the calculations or excess processing. Will need too perform some tests in the ESP32 to see. 
The resulting coefficients as reported by the python script are 

```text
y = -0.606211325872986762 * x + 0.000670906554437763 * x^2 + -0.000000406913425678 * x^3 + 0.000000000120306437 * x^4 + -0.000000000000013855 * x^5 + 491.666480516692104175
y = -6.062113e-01 * x + 6.709066e-04 * x^2 + -4.069134e-07 * x^3 + 1.203064e-10 * x^4 + -1.385464e-14 * x^5 + 4.916665e+02
```

Perhaps calculating partial sums will work better. Did that for an example and tested the code on Linux to confirm appropriate results.

```C++
double calc_temp(int reading) {
    double accumulator;

    accumulator = -1.385464e-14 * reading;
    accumulator = (1.203064e-10 + accumulator) * reading;
    accumulator = (-4.069134e-07 + accumulator) * reading;
    accumulator = (6.709066e-04 + accumulator) * reading;
    accumulator = (-6.062113e-01 + accumulator) * reading;
    accumulator += 4.916665e+02;

    return accumulator;
}
```

Came within a few degrees of the measurement - great! Now to add to a "hello world" example on the ESP32. Done - and result looks great! And performance is more than adequate for this task.

```text
2566 counts 153° measured, 153.1° calculated
2162 counts 180° measured, 178.9° calculated
1643 counts 212° measured, 212.8° calculated
1007 counts 255° measured, 255.4° calculated
 686 counts 286° measured, 284.7° calculated
 262 counts 367° measured, 372.1° calculated
 151 counts 421° measured, 414.1° calculated
 ```
