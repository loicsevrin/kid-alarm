# https://datacarpentry.github.io/image-processing/02-image-basics.html

import imageio.v3 as iio

simba = iio.imread(uri="simba_dort_200.png")

print(simba.shape)

(w,h,c) = simba.shape

c_array = "uint16_t simba[] = {"
for j in range(h):
    for i in range(w):
        # print(pixel)
        r = round(simba[i,j,0] / 255 * 7)
        g = round(simba[i,j,1] / 255 * 7)
        b = round(simba[i,j,2] / 255 * 15)
        # print(simba[i,j,0])
        # print(r,g,b)
        u16 = b << 11
        u16 += r << 5
        u16 += g
        # print(u16)
        c_array += str(u16) + ","
        # c_array += "0xfe6d" + ","
        # c_array += str(1<<((j//2)%16)) + ","
c_array += "};"
print(c_array)