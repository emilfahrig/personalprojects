import mysql.connector

database = mysql.connector.connect(
    host='localhost',
    user='root',
    password='EmilEmil#1',
    database='financials'
)

myCursor = database.cursor()

myCursor.execute('SELECT date FROM {} WHERE ticker="{}"'.format('balance_sheet', 'MSFT'))
available_dates = myCursor.fetchall()

available_dates_list = list()
for date in available_dates:
    date_to_add = date[0]
    if date_to_add not in available_dates_list:
        available_dates_list.append(date_to_add)

print(available_dates_list)

given_date = '6-30-2019'
given_date = given_date.replace('-', '/')

if given_date not in available_dates_list:
    print('Date not found')


