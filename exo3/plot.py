import matplotlib.pyplot as plt

# Read data from the file
processors = []
speedups = []
with open('test.txt', 'r') as file:
    for line in file:
        # Skip lines that don't contain exactly two values
        if len(line.strip().split()) == 2:
            p, s = line.strip().split()
            processors.append(p)
            speedups.append(s)

# Plot the data
plt.plot(processors, speedups, marker='o')
plt.xlabel('Number of Processors')
plt.ylabel('Speedup')
plt.title('Speedup vs Number of Processors')
plt.grid(True)
plt.show()