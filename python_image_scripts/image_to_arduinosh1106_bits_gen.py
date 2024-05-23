import cv2 

screen_size = [64, 64] # width and height should be divisible by 8.

image = cv2.imread('./spider_venom_200x200.bmp')
image = cv2.resize(image, screen_size)
image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
_, image = cv2.threshold(image,100, 255, cv2.THRESH_BINARY)

print(image)
rows, columns = image.shape 
bits_string = 'B'
for row in range(rows):
    for column in range(columns):
        if (column%8 == 0) and (column != 0) :
            bits_string += ',B'
        
        bits_string += '0' if image[row, column] else '1'
    if row < (rows-1):
        bits_string += ',\n'
        bits_string += 'B'

print(bits_string)

cv2.imshow('tet', image)
cv2.waitKey(0)
cv2.destroyAllWindows()