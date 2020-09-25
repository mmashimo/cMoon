
import sys

def julian_date(y, m, d):
	"""
	Compute Julian Date
	"""
	print(y, m, d)
	y1 = 367 * y
	y2_1 = y + int((m+9)/12)
	y2 = int(7 * y2_1 / 4)
	y3 = int(275 * m / 9)
	jd = y1 - y2 + y3 + d + 1721013.5
	print(y1, y2_1, y2, y3)
	print(jd)

	a = y / 100
	b = a / 4
	c = 2 - a + b
	e = int(365.25 * (y + 4716))
	f = int(30.6001 * (m + 1))
	jd = f + c + d + e - 1524.5

	print(jd)

	return jd


def main():
	"""
	Main
	"""
	yr = int(sys.argv[1])
	mon = int(sys.argv[2])
	dy = int(sys.argv[3])

	hr = 18
	min = 45
	sec = 30

	j = julian_date(yr, mon, dy)

	t = (j - 2451545.0) / 36525

	gst = 100.4606184 + (36000.77005361 * t) + (0.00038793 * t * t)

	print(t, gst)

	while gst > 360:
		gst -= 360

	lstMin =  (hr * 60.) + min + (sec / 60.)
	lst = gst + (0.25068447733746215 * lstMin)

	print(gst, lstMin, lst)

	""" Convert lst degrees to 24hour clock (/15 and min/sec) """

if __name__ == '__main__':
	main()