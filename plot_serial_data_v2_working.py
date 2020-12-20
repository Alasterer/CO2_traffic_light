# -*- coding: utf-8 -*-
"""
Spyder Editor

Script for plotting csv data

"""

import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime
from pandas.plotting import register_matplotlib_converters
register_matplotlib_converters()

#-------------------------------------------------------------------------------------------------
# --- DATA ENTRY 
search_string_for_file_detection = '.csv'      # detection string for finding input data
delimiter_sign = ';'                                # sign used in csv as delimiter
column_namescolumn_names=['Date','Time','CO2']  # names of columns (needed for correct column selection)
compensate_offset = False                            # set if offset compensation should be performed
search_string_for_OFFSET_file = '_offset.csv'         # detection string for finding offset data file (file ending)
#output_file_name = 'Result_1.png'          # name of output file (image of plot)
output_file_name = 'temperature_curve.png'          # name of output file (image of plot)
date_format = '%Y-%m-%d'                            # date format used in source data
time_format = '%H:%M:%S.%f'                         # time format used in source data
plot_to_x = 'Time'                                  # choose data for x plot
plot_to_y = 'CO2'                           # choose data for y plot
# selected_columns = (0,1,2,3)                        # select columns to be used from present file (e.g. only use colum 1, 2 and 4 from 10 columns = (0,1,3))
#x_range = [1,200]                                  # Plot range for x axis (remove for auto scaling)
#y_range = [0,3000]                                   # Plot range for y axis (remove for auto scaling)
plot_title = 'Climate chamber temperature profile'  # string to print as plot title
x_lable = 'Time'                                    # string to print on x lable
y_lable = 'Temperature / °C'                        # string to print on y lable
plot_min_max_horizontal_lines = True                # set if min and max horizontal lines will be drawn
amount_skip_header_lines=0                          # amount of lines skipped at start of source files
amount_skip_footer_lines=0                          # amount of lines skipped at end of source files
draw_lines = False                                  # Set if plot should include lines between data points
draw_line_style = '-'                               # Style of line when drawn: 
                                                    #  '-' = solid line, ':' = dotted line, '--' = dashed line, '-.' = dash dotted line
draw_symbols = True                                 # Set if plot should include symbols at data points
draw_symbol_style = '.'                             # Style of symbol at data point
                                                    #  '.' 	point marker, ',' 	pixel marker, 'o' 	circle marker, 'v' 	triangle_down marker
                                                    #  '^' 	triangle_up marker, '<' 	triangle_left marker, '>' 	triangle_right marker
                                                    #  '1' 	tri_down marker, '2' 	tri_up marker, '3' 	tri_left marker, '4' 	tri_right marker
                                                    #  's' 	square marker, 'p' 	pentagon marker, '*' 	star marker, 'h' 	hexagon1 marker
                                                    #  'H' 	hexagon2 marker, '+' 	plus marker, 'x' 	x marker, 'D' 	diamond marker
                                                    #  'd' 	thin_diamond marker, '|' 	vline marker, '_' 	hline marker
#-------------------------------------------------------------------------------------------------

# Get path of this script and search for files with certain strings in file name
cur_dir = os.getcwd()
files = []
offset_file = ""
for file in os.listdir(cur_dir):
    if file.endswith(search_string_for_file_detection):
        files.append(file)
        print(file)
    else:
        if compensate_offset:
            if file.endswith(search_string_for_OFFSET_file):
                offset_file = file
                print('Offset file: ' + offset_file)
            
# Read offset file
if (compensate_offset and offset_file!=''):
    offset_data = np.genfromtxt(offset_file, dtype=None)
elif (compensate_offset and offset_file==''):
    print('Offset actived but offset file NOT found! --> Offset = 0 used for all data.')
    offset_data = [0]*len(files)
else:
    offset_data = [0]*len(files)

# Functions for converting date and time
#old: #str2date = lambda x: datetime.strptime(x.decode("utf-8"), '%Y-%m-%d %H:%M:%S')
#str2date = lambda x: datetime.strptime(x.decode("utf-8"), date_format)
#str2time = lambda x: datetime.strptime(x.decode("utf-8"), time_format)
#str2timeAlternative = lambda x: datetime.strptime(x.decode("utf-8"), '%H:%M:%S')
dateAndTimeToDatetime = lambda x: datetime.combine(datetime.date(x[0]), datetime.time(x[1]))

def str2date(x):
    return_x = 0
    try:
        return_x = datetime.strptime(x.decode("utf-8"), date_format)
    except ValueError:  # e.g. if date is written differently, like american style date
        return_x = datetime.strptime(x.decode("utf-8"), '%m-%d-%Y')
    return return_x

def str2time(x):
    return_x = 0
    try:
        return_x = datetime.strptime(x.decode("utf-8"), time_format)
    except ValueError:  # e.g. if time has no decimal places then test time conversion without decimal places
        return_x = datetime.strptime(x.decode("utf-8"), '%H:%M:%S')
    return return_x

def convert_decimal(x):
    return x.replace(',', '.').encode()

# Define plot line style according to selection if lines are drawn (draw_lines) and what style is used (draw_line_style)
if draw_lines:
    chosen_linestyle = draw_line_style
else:
    chosen_linestyle = 'None'

# Define plot style according to chosen variable chosen_style    
if draw_symbols:
    chosen_symbol_style = draw_symbol_style
else:
    chosen_symbol_style = 'None'

# Define colors for plots
colors = ['#1f77b4',
          '#ff7f0e',
          '#2ca02c',
          '#d62728',
          '#9467bd',
          '#8c564b',
          '#e377c2',
          '#7f7f7f',
          '#bcbd22',
          '#17becf',
          '#1a55FF',
          '#3D3D3D']
          #'#000000']

#input_file = r'H:\Working_Directory\Programming\Python\Plot_CSV_data\EHCTS-Arduino Nano-20190529-0603_data.csv'
#data = np.genfromtxt(input_file, dtype=None, delimiter=';', skip_header=0, skip_footer=0, names=['date','time','dummy','Temperature'], converters = {0: str2date,1: str2time})

# Setup plot parameters
fig = plt.figure(figsize=(9,7))
plt.title(plot_title)
ax1 = fig.add_subplot(111)
ax1.set_xlabel(x_lable, size=14)
ax1.set_ylabel(y_lable, size=14)
ax1.xaxis_date()

# Check if x range is present and use it if it is
try:
    x_range
except NameError:   # variable is not defined
    None
else:               # variable IS defined
    ax1.set_xlim(x_range)

# Check if x range is present and use it if it is
try:
    y_range
except NameError:   # variable is not defined
    None
else:               # variable IS defined
    ax1.set_ylim(y_range)    

# Create columns vector including all data if none is defined
try:
    selected_columns
except NameError:   # variable is not defined
    selected_columns = range(len(column_namescolumn_names))
    pass

# Read data of all files that were found
# & Combine date and time cells into one cell and empty date cell since not needed any longer
# & Add data to plot
y_min = 99999999
y_max = -99999999

for current_file, current_color, offset_value in zip(files, colors, offset_data):
    data = np.genfromtxt((convert_decimal(x) for x in open(current_file)), usecols=selected_columns, dtype=None, delimiter=delimiter_sign, skip_header=amount_skip_header_lines, skip_footer=amount_skip_footer_lines, names=column_namescolumn_names, converters = {0: str2date,1: str2time})
    for line in data:   # Combine date and time cells into one cell and empty date cell since not needed any longer
        line[1] = dateAndTimeToDatetime((line[0],line[1]))
        line[0] = 0
    ax1.plot(data[plot_to_x], data[plot_to_y]-offset_value, linestyle=chosen_linestyle, color=current_color, marker=chosen_symbol_style, markerfacecolor='none', label=current_file.replace(search_string_for_file_detection,'').replace('_',' '))     #, alpha=0.4)        

    for dataline in data:
        if dataline[plot_to_y]-offset_value > y_max:
            y_max = dataline[plot_to_y]-offset_value
        if dataline[plot_to_y]-offset_value < y_min:
            y_min = dataline[plot_to_y]-offset_value

 
#ax1.plot(data['time'], data['Temperature'], 'r.', markerfacecolor='none', label=plot_lable)
#ax1.plot(data2['time'], data2['PD_input'], 'bo', markerfacecolor='none', label='data 2')

# Setup some more plot parameters
myFmt = mdates.DateFormatter('%Y-%m-%d\n%H:%M:%S')
ax1.xaxis.set_major_formatter(myFmt)
fig.autofmt_xdate()
plt.grid(True, color='#EEEEEE', linestyle='-', linewidth=1)
ax1.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0.)
ax1.axhline(y_min, color='#FFD3DA', lw=1, alpha=0.7)    # draw horizontal line for minimum
ax1.axhline(y_max, color='#FFD3DA', lw=1, alpha=0.7)    # draw horizontal line for maximum

# Save and show plot data
plt.savefig(output_file_name, bbox_inches='tight', dpi=300)
plt.show()
