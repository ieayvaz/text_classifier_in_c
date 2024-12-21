import matplotlib.pyplot as plt

# Function to read data from the file
def read_data(file_path):
    epochs = []
    losses = []
    with open(file_path, 'r') as file:
        for line in file:
            epoch, loss = map(float, line.strip().split())
            epochs.append(epoch)
            losses.append(loss)
    return epochs, losses

# File path
file_path = 'adamepochloss.txt'

# Read data from the file
epochs, losses = read_data(file_path)

# Plotting
plt.plot(epochs, losses, marker='o', linestyle='-')
plt.title('Epoch vs Loss')
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.grid(True)
plt.show()
