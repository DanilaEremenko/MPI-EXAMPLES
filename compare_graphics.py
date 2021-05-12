import plotly
import json

methods = [
    "sequential",
    "posix",
    "mpi"
]
res_path = 'res/res_compare_5_repeat.json'
MAX_PROC_NUM = 100
MIN_PROC_NUM = 2
###############################################################
# --------------------------- run -----------------------------
###############################################################
import subprocess

res_dict = {}

for method in methods:
    binary_path = f"cmake-build-debug/main_{method}"
    res_json = f"res/res_{method}.json"
    res_dict[method] = {'proc_num': [], 'time': []}
    for proc_num in list(range(MIN_PROC_NUM, MAX_PROC_NUM, 1)):
        curr_time = []
        for repeat_num in range(5):
            print(f"method = {method}, proc_num = {proc_num}, i = {repeat_num}".upper())
            if method == 'mpi':
                subprocess.call(["mpiexec", "-np", str(proc_num + 1), binary_path], stdout=subprocess.DEVNULL)
            else:
                subprocess.call([binary_path, str(proc_num)], stdout=subprocess.DEVNULL)

            with open(res_json) as fp:
                curr_res = json.load(fp=fp)
                curr_time.append(curr_res['input_list'][0]['time'])
        res_dict[method]['proc_num'].append(proc_num)
        res_dict[method]['time'].append(sum(curr_time) / len(curr_time))

with open(res_path, 'w') as fp:
    json.dump(res_dict, fp)

###############################################################
# --------------------------- visualize -----------------------
###############################################################
# from plotly import graph_objects as go
# from plotly.subplots import make_subplots
#
# with open(res_path, 'r') as fp:
#     res_dict = json.load(fp)
#
# fig = make_subplots(
#     rows=len(methods),
#     cols=1,
#     # subplot_titles=methods
# )
#
# for repeat_num, method in enumerate(methods):
#     fig.add_trace(
#         go.Scatter(
#             x=res_dict[method]['proc_num'],
#             y=res_dict[method]['time'],
#             name=method,
#         ),
#         row=1,
#         col=1
#     )
#
# fig.update_layout(
#     height=512 * len(methods),
#     xaxis_title="proc_num",
#     yaxis_title="time",
# )
# fig.update_xaxes(
#     range=(
#         MIN_PROC_NUM - 2,
#         MAX_PROC_NUM + 2,
#     )
# )
#
# fig.show()
