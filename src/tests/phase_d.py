import matplotlib.pyplot as plt

# Define the data
M_values = [2, 3, 4, 5]
critical_points = [2.056008935464906, 2.624206629061214, 3.190114834094382, 3.799517068220771]
errors = [0.0018052727696703443, 0.0008050062400761917, 0.00035936786321159777, 7.991029201272262e-05]

# Plot with error bars
# fmt='o-' creates a line with circular markers
# capsize adds the small horizontal lines at the top and bottom of the error bars
plt.errorbar(critical_points,M_values, xerr=errors, fmt='o', 
             capsize=5, capthick=2, color='black', 
             markerfacecolor='red', markeredgecolor='red', markersize=8)

# Add labels, title, and formatting
plt.xlabel('z', fontsize=12)
plt.ylabel('M', fontsize=12)

# Ensure only the discrete M values are shown on the x-axis
plt.yticks(M_values)
# Add a grid for easier reading
plt.grid(True, linestyle='--', alpha=0.7)


# Adjust layout to prevent clipping and display/save the plot
plt.tight_layout()
plt.savefig('phase_diagram.png', dpi=300, bbox_inches='tight')
plt.show()