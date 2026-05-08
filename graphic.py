import matplotlib.pyplot as plt

digits = [10, 100, 1000, 10000, 100000, 1000000]

gl_cycles = [26021, 44076, 255679, 9792155, 319106626, 6294123323]
ch_cycles = [3633, 13393, 124717, 2739609, 77962703, 1643310395]

gl_errors = [974, 775, 22048, 8878, 190330, 90155309]
ch_errors = [19, 30, 2045, 5357, 163982, 93850129]

plt.figure(figsize=(10, 7))

plt.errorbar(digits, gl_cycles, yerr=gl_errors, fmt='-o', color='red', 
             capsize=5, capthick=2, elinewidth=2, markersize=6, 
             label='Gauss-Legendre')

plt.errorbar(digits, ch_cycles, yerr=ch_errors, fmt='-s', color='blue', 
             capsize=5, capthick=2, elinewidth=2, markersize=6, 
             label='Chudnovsky (Binary Splitting)')

plt.xscale('log')
plt.yscale('log')

plt.xlabel('Количество вычисляемых знаков $\pi$', fontsize=12)
plt.ylabel('Количество тактов процессора', fontsize=12)
# plt.title('Сравнение скорости алгоритмов вычисления числа Пи', fontsize=14, fontweight='bold')

plt.grid(True, which="major", linestyle="-", alpha=0.7)
plt.grid(True, which="minor", linestyle="--", alpha=0.3)

plt.legend(fontsize=12, loc='upper left')

plt.tight_layout()

plt.savefig('pi_benchmark_loglog.png', dpi=300)

plt.show()
