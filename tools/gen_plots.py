import subprocess

input_data = 'abc'

for i, regex in enumerate(['^(abc)', '^(a|bc)', '^(a+bc)', '^(a*bc)', '^((a+)+bc)', '^((a+)*bc)', '^((a*)+bc)', '^((a*)*bc)']):
    print(i, regex)
    command = ['../build/src/retoy', '-r', regex]
    process = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    process.stdin.write(input_data + "\n")
    stdout, stderr = process.communicate()
    
    command = ['python3', 'plot_stats.py', regex, input_data, f"/tmp/plot_{i}.png"]
    
    print(command)
    subprocess.check_output(command)   
