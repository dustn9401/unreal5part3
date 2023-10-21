import csv

csv_path = "ABCharacterStatTable.csv"

num_row = 10

with open(csv_path, 'w', newline='', encoding='utf-8') as file:
    csv_writer = csv.writer(file)

    header = ['Name', 'MaxHp', 'Attack', 'AttackRange', 'AttackSpeed', 'MovementSpeed']
    csv_writer.writerow(header)

    for i in range(num_row):
        data = [f'CLVL{i+1}', 100 + 50 * i, 100 + 20 * i, 40, round(1 + 0.05 * i, 2), 400]
        csv_writer.writerow(data)
