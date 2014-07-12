# Nicholas Jordan
# CS 311
# Assignment 2 Part 6


f = open("numbers", "r")
nums = f.read()

greatest = 0
digits = []

for x in range(len(nums)-5):
  current = int(nums[x])* int(nums[x+1])* int(nums[x+2])* int(nums[x+3])* int(nums[x+4])
  if current >= greatest:
      greatest = current
      digits = [int(nums[x]), int(nums[x+1]), int(nums[x+2]), int(nums[x+3]), int(nums[x+4])]

print greatest
print digits

f.close()
    
