# https://datacarpentry.github.io/image-processing/02-image-basics.html

import imageio.v3 as iio

# filename = "pumbaa_asleep"
filename = "pumbaa_hat"

image = open(filename+"_150.png", mode='rb') 
imageContent = image.read()
# print(imageContent)


f = open("kid_alarm/"+filename+".h", "w")
f.write(f"static const unsigned char {filename}[] PROGMEM = {{\n")
nb_bytes = 0
for byte in imageContent:
    f.write(f"{byte}, ")
    nb_bytes += 1
f.write(f"\n}}\n")
f.close()
print(f"{nb_bytes} bytes written")