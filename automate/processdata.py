# This Python file uses the following encoding: utf-8

if __name__ == "__main__":
    f = open("../traces/test/mmWave-tcp-data0.txt", "r")
    w = open("../traces/test/processedwindow.txt", "w")
    lines = f.readlines()
    sum = 0.0
    nextTime = 0.1
    for line in lines:
        time, value = line.split()
        time = float(time)
        value = int(value)
        if time < nextTime:
            sum = sum + value
        else:
            sum = 10.0 * sum / (1024.0 * 1024.0)
            timeStr = str(time - 0.1)
            sumStr = str(sum)
            outLine = timeStr + "; " + sumStr + "\n"
            w.write(outLine)
            sum = value
            nextTime = nextTime + 0.1

    f.close()
    w.close()
