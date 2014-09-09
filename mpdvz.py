import os
import audioop
import time
import errno
import math

fifo = os.open('/tmp/mpd.fifo', os.O_RDONLY)

WANTED_BYTES = 4096

left_min = -30
left_max = 20
span = left_max - left_min

while True:
    our_bytes = ''
    try:
        while len(our_bytes) < WANTED_BYTES:
            our_bytes += os.read(fifo, WANTED_BYTES)
    except OSError as err:
        if err.errno == errno.EAGAIN or err.errno == errno.EWOULDBLOCK:
            our_bytes = None
        else:
            raise

    if our_bytes:
        left_chan = audioop.tomono(our_bytes, 2, 1, 0)
        right_chan = audioop.tomono(our_bytes, 2, 0, 1)
        stereo_peak = audioop.max(our_bytes, 2)
        left_peak = audioop.max(left_chan, 2)
        right_peak = audioop.max(right_chan, 2)
        left_db = 20 * math.log10(left_peak) -74
        right_db = 20 * math.log10(right_peak) -74

        if left_db > left_max:
            left_max = left_db
            span = left_max - left_min
        if left_db < left_min:
            left_min = left_db
            span = left_max - left_min

        amount = (left_db - left_min) / (left_max - left_min)

        count = int(math.floor(amount / 0.05))

        print "#" * count


