from calcStm32Crc import CalcStm32Crc as CalcStm32Crc
import numpy as np


def printCrc(string):
    calc_crc = CalcStm32Crc()
    uint32_buff = calc_crc.string_to_uint32_list(string)
    crc: np.uint = calc_crc.hal_crc_calculate(uint32_buff)
    print(f"{string}: {crc}")


if __name__ == '__main__':
    printCrc("e2dumpjournalmap") #  2362017329


    # printCrc("setSensId")  # 2358927868
    # printCrc("getSensIdTable")  # 3913104177
    # printCrc("setStationId")  # 1925253141
    # printCrc("getStationId")  # 2948963465
    # printCrc("clrSensIdTable")  # 4035361305
    # printCrc("calcHash")  # 1050090700
    # printCrc("shutup")  # 959926194
    # printCrc("talk")  # 1639364146
    # printCrc("reboot")  # 1676458703
    # printCrc("exit")  # 1973435441
    #
    # calcStm32Crc = CalcStm32Crc()
    # calcStm32Crc.resetCrc()
    # crc = calcStm32Crc.calcCrcUpdate(0x11223344)
    # expect = 0xB14257CC
    # print(f"0x11223344: {crc} / {expect}")

