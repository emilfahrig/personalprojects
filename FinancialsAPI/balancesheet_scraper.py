from bs4 import BeautifulSoup
import pandas as pd
import requests
import mysql.connector
import numpy as np
import pprint

# establish connection to the financials mySQL database
balance_sheetDatabase = mysql.connector.connect(
    host='localhost',
    user='root',
    password='EmilEmil#1',
    database='financials'
)

# create a cursor we can use to access the database
db_cursor = balance_sheetDatabase.cursor()

# create a table to hold the data in the balance sheet
# db_cursor.execute('CREATE TABLE balance_sheet (date VARCHAR(100), financial_type VARCHAR(100), value VARCHAR(100), ticker VARCHAR(100), INDEX date_type (date, financial_type, ticker))')
# balance_sheetDatabase.commit()

# create a formula we can use to insert values into the balance_sheet table in mySQL
insert_formula = 'INSERT INTO balance_sheet (date, financial_type, value, ticker) VALUES (%s, %s, %s, %s)'

# create the list of stock tickers for which we want the financial information to be available for
dow_tickers = ['MMM', 'AXP', 'AAPL', 'BA', 'CAT', 'CVX', 'CSCO', 'KO', 'DIS', 'DD', 'XOM', 'GE', 'GS', 'HD', 'IBM',
               'INTC', 'JNJ', 'JPM', 'MCD', 'MRK', 'MSFT', 'NKE', 'PFE', 'PG', 'TRV', 'RTX', 'UNH', 'VZ', 'V', 'WMT']

base_url = 'https://finance.yahoo.com/quote/{}/balance-sheet'

for stock in dow_tickers:
    print('Scraping from ticker ' + stock + '...')
    balance_sheet_info = requests.get(base_url.format(stock))
    balance_sheet_scraper = BeautifulSoup(balance_sheet_info.content, 'html.parser')

    dates_info = balance_sheet_scraper.find('div', class_='D(tbr) C($primaryColor)')
    reported_dates = dates_info.find_all('span')

    dates = list()
    for date in reported_dates:
        dates.append(date.text)

    dates.remove('Breakdown')

    balance_sheet_values = balance_sheet_scraper.find('div', class_='D(tbrg)')

    balance_sheet_items = balance_sheet_values.find_all('span', class_='Va(m)')

    reported_items = list()
    for item in balance_sheet_items:
        reported_items.append(item.text)

    reported_values = balance_sheet_values.find_all('div', class_=['Ta(c) Py(6px) Bxz(bb) BdB Bdc($seperatorColor) Miw(120px) Miw(140px)--pnclg D(tbc)', 'Ta(c) Py(6px) Bxz(bb) BdB Bdc($seperatorColor) Miw(120px) Miw(140px)--pnclg Bgc($lv1BgColor) fi-row:h_Bgc($hoverBgColor) D(tbc)'])

    reported_values_list = list()
    for value in reported_values:
        reported_values_list.append(value.text)

    reported_values_list = np.array(reported_values_list)
    reported_values_list_reshaped = reported_values_list.reshape(len(balance_sheet_items), len(dates))

    balance_sheet_stock_data = pd.DataFrame(reported_values_list_reshaped, index=reported_items, columns=dates)

    for date in balance_sheet_stock_data.columns:
        for item in balance_sheet_stock_data.index:
            reported_value = balance_sheet_stock_data[date].loc[item]
            reported_value = str(reported_value)
            tuple_to_insert = (date, item, reported_value, stock)
            db_cursor.execute(insert_formula, tuple_to_insert)

balance_sheetDatabase.commit()
