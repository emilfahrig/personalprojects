import mysql.connector
import pandas as pd
import requests
from bs4 import BeautifulSoup
import numpy as np

# initialize access to the mySQL database
financialsDatabase = mysql.connector.connect(
    host='localhost',
    user='root',
    password='EmilEmil#1',
    database='financials'
)

myCursor = financialsDatabase.cursor()

# create a database to store the financial, the date the financial was reported on and the value that was reported,
# and index date_type has also been created for quick database lookup
# myCursor.execute('CREATE TABLE reported_financials (date VARCHAR(100), financial_type VARCHAR(100), value VARCHAR(100), ticker VARCHAR(100), INDEX date_type (date, financial_type, ticker))')
# financialsDatabase.commit()

# create a formula that will be used to insert values into the mySQL database
insert_formula = 'INSERT INTO reported_financials (date, financial_type, value, ticker) VALUES (%s, %s, %s, %s)'

# create the list of stock tickers for which we want the financial information to be available for
dow_tickers = ['MMM', 'AXP', 'AAPL', 'BA', 'CAT', 'CVX', 'CSCO', 'KO', 'DIS', 'DD', 'XOM', 'GE', 'GS', 'HD', 'IBM',
               'INTC', 'JNJ', 'JPM', 'MCD', 'MRK', 'MSFT', 'NKE', 'PFE', 'PG', 'TRV', 'RTX', 'UNH', 'VZ', 'V', 'WMT']

# the base url from which we will be scraping information from
url = 'https://finance.yahoo.com/quote/{}/financials'

# create an empty dataframe we will use to store all financial information for each ticker
financial_data = pd.DataFrame()

for stock in dow_tickers:
    print('Scraping financials for ticker ' + stock + '...')
    
    # create an instance of Beautiful Soup to scrape the url of the current stock for financial information
    url_request = requests.get(url.format(stock))
    yahoo_finance_financials_table = BeautifulSoup(url_request.content, 'html.parser')
    info = yahoo_finance_financials_table.find('div', class_='W(100%) Whs(nw) Ovx(a) BdT Bdtc($seperatorColor)')

    # scrape the dates for which the financial information was presented on
    reporting_dates = info.find('div', class_='D(tbr) C($primaryColor)')
    dates = reporting_dates.find_all('span')

    # add all dates to a list and remove Breakdown with is not necessary
    reporting_dates_list = list()
    for date in dates:
        reporting_dates_list.append(date.text)

    reporting_dates_list.remove('Breakdown')

    # now, scrape all reported financial information reported on yahoo (Net income, Revenue, etc...)
    financial_information = info.find_all('span', class_='Va(m)')

    # add the financial info to a list
    financial_info_list = list()
    for category in financial_information:
        financial_info_list.append(category.text)

    # now scrape all financial values from the url
    financial_figures = info.find_all('div', class_=['Ta(c) Py(6px) Bxz(bb) BdB Bdc($seperatorColor) Miw(120px) Miw(140px)--pnclg Bgc($lv1BgColor) fi-row:h_Bgc($hoverBgColor) D(tbc)', 'Ta(c) Py(6px) Bxz(bb) BdB Bdc($seperatorColor) Miw(120px) Miw(140px)--pnclg D(tbc)'])

    # add the values to a list
    reported_values_list = list()
    for figure in financial_figures:
        reported_values_list.append(figure.text)

    # reshape the financial values so they can be added to the DataFrame
    reported_financial_data = np.array(reported_values_list).reshape(len(financial_info_list),
                                                                     len(reporting_dates_list))

    # make a Dataframe with the financial information of the current stock
    stock_financials = pd.DataFrame(reported_financial_data, columns=reporting_dates_list,
                                    index=financial_info_list)

    # now insert all values stored in the DataFrame into the mySQL database
    for date in stock_financials.columns:
        for financial_type in stock_financials.index:
            reported_value = str(stock_financials[date].loc[financial_type])
            tuple_to_insert = (date, financial_type, reported_value, stock)
            myCursor.execute(insert_formula, tuple_to_insert)

# make sure all changes made to the table take effect
financialsDatabase.commit()
