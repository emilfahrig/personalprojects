import requests
from bs4 import BeautifulSoup
import pandas as pd
import mysql.connector
import numpy as np

cash_flow_database = mysql.connector.connect(
    host='localhost',
    user='root',
    password='EmilEmil#1',
    database='financials'
)

database_cursor = cash_flow_database.cursor()

# database_cursor.execute('CREATE TABLE cash_flow (date VARCHAR(100), financial_type VARCHAR(100), value VARCHAR(100), ticker VARCHAR(100), INDEX date_type (date, financial_type, ticker))')
# cash_flow_database.commit()

insert_formula = 'INSERT INTO cash_flow (date, financial_type, value, ticker) VALUES (%s, %s, %s, %s)'

dow_tickers = ['MMM', 'AXP', 'AAPL', 'BA', 'CAT', 'CVX', 'CSCO', 'KO', 'DIS', 'DD', 'XOM', 'GE', 'GS', 'HD', 'IBM',
               'INTC', 'JNJ', 'JPM', 'MCD', 'MRK', 'MSFT', 'NKE', 'PFE', 'PG', 'TRV', 'RTX', 'UNH', 'VZ', 'V', 'WMT']

base_url = 'https://finance.yahoo.com/quote/{}/cash-flow'

for ticker in dow_tickers:
    print('Scraping cash flow values from ticker ' + ticker + '...')
    cash_flow_info = requests.get(base_url.format(ticker))
    cash_flow_scraper = BeautifulSoup(cash_flow_info.content, 'html.parser')

    dates_info = cash_flow_scraper.find('div', class_='D(tbhg)')

    dates = dates_info.find_all('span')

    dates_list = list()
    for date in dates:
        dates_list.append(date.text)

    cash_flow_values = cash_flow_scraper.find('div', class_='D(tbrg)')
    cash_flow_items = cash_flow_values.find_all('div', class_=['Ta(c) Py(6px) Bxz(bb) BdB Bdc($seperatorColor) Miw(120px) Miw(140px)--pnclg Bgc($lv1BgColor) fi-row:h_Bgc($hoverBgColor) D(tbc)', 'Ta(c) Py(6px) Bxz(bb) BdB Bdc($seperatorColor) Miw(120px) Miw(140px)--pnclg D(tbc)', 'D(ib) Va(m) Ell Mt(-3px) W(215px)--mv2 W(200px) undefined'])

    cash_flow_values_list = list()
    for item in cash_flow_items:
        cash_flow_values_list.append(item.text)

    num_columns = int(len(dates_list))
    num_rows = int(len(cash_flow_values_list) / num_columns)

    cash_flow_values_list = np.array(cash_flow_values_list)
    cash_flow_values_list_reshaped = cash_flow_values_list.reshape(num_rows, num_columns)

    cash_flow_reported_values = pd.DataFrame(cash_flow_values_list_reshaped, columns=dates_list)
    cash_flow_reported_values.index = cash_flow_reported_values['Breakdown']
    cash_flow_reported_values = cash_flow_reported_values.drop(['Breakdown'], axis=1)

    for date in cash_flow_reported_values.columns:
        for item in cash_flow_reported_values.index:
            reported_value = cash_flow_reported_values[date].loc[item]
            reported_value = str(reported_value)
            tuple_to_insert = (date, item, reported_value, ticker)
            database_cursor.execute(insert_formula, tuple_to_insert)

cash_flow_database.commit()
