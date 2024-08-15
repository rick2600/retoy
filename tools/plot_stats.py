import matplotlib.pyplot as plt
import json
import sys

# Print the stats (number of threads during execution) for the last execution

FILENAME = '/tmp/retoy.stats.json'


def main():
    regex = None
    text = None
    img = None
    if len(sys.argv) > 2:
        regex = sys.argv[1]
        text = sys.argv[2]

    if len(sys.argv) > 3:
        img = sys.argv[3]



    threads_data = []
    with open(FILENAME) as f: threads_data = json.load(f)
    time_data = list(range(len(threads_data)))
    plt.plot(time_data, threads_data)

    if regex and text:
        plt.text(
            0.05, 0.9,
            f'regex: {regex}\ntext: {text}',
            fontsize=10, color='red', transform=plt.gca().transAxes
        )

    plt.xticks([])
    plt.ylim(0, max(threads_data) * 2)

    plt.xlabel('Time')
    plt.ylabel('Threads')
    plt.title('Threads vs. Time')

    if img:
        plt.savefig(img)
    else:
        plt.show()

if __name__ == "__main__":
    main()