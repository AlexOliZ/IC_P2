# Python programming for Golomb Encoding
import math
  
# taking input for N and  M where 
# M == 2 ^ n or M != 2 ^ n
N = 37
M = 11
  
# for finding the value of preceding 
# number of zeros by dividing N by M
q = N//M
# for computing the remainder of N by M
r = N % M
  
# appending that many numbers of zeros in
# starting of the encoded code initially 
quo ='0'*q+'1'
  
# for computing the value of b ie floor of 
# log(M) base 2 which will be used for computing value of k
b = math.floor(math.log2(M))
k = 2**(b + 1)-M
# upon comparing the value of remainder with the 
# value of k if less we we convert remainder r to 
# binary and add the value from # index 2 because 
# at index 0, 1 "0b" is present
if r < k:
    rem = bin(r)[2:]
    print(bin(r)[2:])
    l = len(rem)
      
# upon the calculating value of rem if it is less than
# computed value of b we add b-1 number of zeros in
# preceding of the # remainder
    if l<b:
        rem = '0'*(b-l)+rem
else:
# we convert remainder r to binary and add the 
# value from index 2 because at index 0, 1 "0b" is present
    rem = bin(r + k)[2:]
    l = len(rem)
# upon calculating value of rem if it is less than
# computed value of b we add b-1 number of zeros in
# preceding of the # remainder
    if l<b + 1:
        rem = '0'*(b + 1-l)+rem
golomb_code = quo + rem
print("The golomb code encoding for x = {} and b = {} is {}".
      format(N, M, golomb_code))