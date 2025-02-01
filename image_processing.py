# https://datacarpentry.github.io/image-processing/02-image-basics.html

import imageio.v3 as iio

# filename = "pumbaa_asleep"
filename = "pumbaa_hat"

# img_src = iio.imread(uri=filename+".png")
# img_src = iio.imread(uri=filename+"_200.jpg")
img_src = iio.imread(uri=filename+"_150.jpg")

print(img_src.shape)

(h,w,c) = img_src.shape

c_array = "uint16_t "+filename+"[] = {"
image_size_divider = 1
for i in range(0,h,image_size_divider):
    for j in range(0,w,image_size_divider):
        # print(pixel)
        r = round(img_src[i,j,0] / 255 * 7)
        g = round(img_src[i,j,1] / 255 * 7)
        b = round(img_src[i,j,2] / 255 * 7)
        # print(img_src[i,j,0])
        # print(r,g,b)
        
        u16 = b << 10
        u16 += r << 5
        u16 += g
        
        # u16 = r << 11
        # u16 += g << 5
        # u16 += b
        
        # print(u16)
        c_array += str(u16) + ","
        # c_array += "0xfe6d" + ","

        # if j<2 or i<2:
        #     c_array += str(0xffff) + ","
        # else: 
        #     c_array += str(1<<(15-((j//10)%16))) + ","

c_array += "};"
# print(c_array)
print(f"out size = {w//image_size_divider},{h//image_size_divider}")
f = open("kid_alarm/"+filename+".h", "w")
f.write(c_array)
f.close()