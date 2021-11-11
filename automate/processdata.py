# This Python file uses the following encoding: utf-8

if __name__ == "__main__":
    f = open("../traces/test/mmWave-tcp-data0.txt", "r")
    w = open("../traces/test/processedwindow.txt", "w")
    lines = f.readlines()
    sum = 0.0
    nextTime = 0.1
    startTime = 0.0
    lastTime = 0.1
    for line in lines:
        time, value = line.split()
        time = float(time)
        lastTime = time
        value = int(value)
        if time < nextTime:
            sum = sum + value
        else:
            if lastTime - startTime < 0.001:
                sum = 0.0
            else:
                sum = sum / (1024.0 * 1024.0 * (lastTime - startTime))
            timeStr = str(time - 0.1)
            sumStr = str(sum)
            outLine = timeStr + "; " + sumStr + "\n"
            w.write(outLine)
            sum = value
            nextTime = nextTime + 0.1
            startTime = time

    f.close()
    w.close()

