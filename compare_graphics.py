import json

methods = [
    "sequential",
    "posix",
    "mpi"
]
MAX_PROC_NUM = 202
MIN_PROC_NUM = 2

MEAN_BY = 7
###############################################################
# --------------------------- run -----------------------------
###############################################################
import time

for mean in (MEAN_BY,):
    res_path = f"res/res_fair_compare_to_{MAX_PROC_NUM}_mean_{mean}.json"

    import subprocess

    res_dict = {}

    for method in methods:
        binary_path = f"cmake-build-debug/main_{method}"
        res_json = f"res/res_{method}.json"
        res_dict[method] = {'proc_num': [], 'time': []}
        for proc_num in list(range(MIN_PROC_NUM, MAX_PROC_NUM, 2)):
            curr_time_list = []
            for repeat_num in range(mean):
                print(f"method = {method}, proc_num = {proc_num}, i = {repeat_num}".upper())
                start_time = time.time()
                try:
                    if method == 'mpi':
                        subprocess.check_output(["mpiexec", "-np", str(proc_num + 1), binary_path])
                    else:
                        subprocess.check_output([binary_path, str(proc_num)])
                    curr_time_list.append(time.time() - start_time)
                except subprocess.CalledProcessError as e:
                    raise Exception(f"Test failed for {method}")

            res_dict[method]['proc_num'].append(proc_num)
            res_dict[method]['time'].append(curr_time_list)

    with open(res_path, 'w') as fp:
        json.dump(res_dict, fp)

###############################################################
# --------------------------- visualize -----------------------
###############################################################
from plotly import graph_objects as go
from plotly.subplots import make_subplots
import numpy as np
import math

res_files = (
    'res/res_fair_compare_to_202_mean_7.json',
)

for i, res_path in enumerate(res_files):
    with open(res_path, 'r') as fp:
        res_dict = json.load(fp)

    fig_time = go.Figure()
    fig_disp = go.Figure()

    for repeat_num, (method) in enumerate(methods):
        # y_mean_arr = [sum(curr_times) / len(curr_times) for curr_times in res_dict[method]['time']]
        y_mean_arr = [sorted(curr_times)[4] for curr_times in res_dict[method]['time']]  # robust one

        disp_arr = [math.sqrt(sum((np.array(curr_times) - curr_mean) ** 2) / len(curr_times))
                    for curr_times, curr_mean in zip(res_dict[method]['time'], y_mean_arr)]

        # y_upper = [max(curr_times) for curr_times in res_dict[method]['time']]
        # y_lower = [min(curr_times) for curr_times in res_dict[method]['time']]
        # y_upper = [mean + disp for mean, disp in zip(y_mean_arr, disp_arr)]
        # y_lower = [mean - disp for mean, disp in zip(y_mean_arr, disp_arr)]
        y_upper = [sorted(curr_times)[len(curr_times) - 2] for curr_times in res_dict[method]['time']]
        y_lower = [sorted(curr_times)[1] for curr_times in res_dict[method]['time']]

        color_rgb = [0] * 3
        color_rgb[repeat_num] = 180
        color_rgb = [str(n) for n in color_rgb]
        rgb_str = f"rgb({','.join(color_rgb)})"
        rgba_str = f"rgba({','.join(color_rgb)}, 0.2)"

        fig_disp.add_trace(
            go.Scatter(
                x=res_dict[method]['proc_num'],
                y=disp_arr,
                mode="markers+lines",
                name=method,
                line=dict(color=rgb_str)
            )
        )

        fig_time.add_trace(
            go.Scatter(
                x=res_dict[method]['proc_num'],
                y=y_mean_arr,
                mode="markers+lines",
                name=method,
                line=dict(color=rgb_str)
            )
        )

        # confidence interval
        fig_time.add_trace(
            go.Scatter(
                x=res_dict[method]['proc_num'] + res_dict[method]['proc_num'][::-1],  # x, then x reversed
                y=y_upper + y_lower[::-1],
                # upper, then lower reversed
                fill='toself',
                fillcolor=rgba_str,
                line=dict(color='rgba(255,255,255,0)'),
                hoverinfo="skip",
                showlegend=False
            )
        )

    fig_disp.update_layout(
        height=1024,
        width=2048,
        xaxis_title="proc_num",
        yaxis_title="dispersion",
        title=res_path
    )
    fig_disp.update_xaxes(
        range=(
            MIN_PROC_NUM - 2,
            MAX_PROC_NUM + 2,
        )
    )
    fig_disp.show()

    fig_time.update_layout(
        height=1024,
        width=2048,
        xaxis_title="proc_num",
        yaxis_title="time",
        title=res_path
    )
    fig_time.update_xaxes(
        range=(
            MIN_PROC_NUM - 2,
            MAX_PROC_NUM + 2,
        )
    )
    fig_time.show()
