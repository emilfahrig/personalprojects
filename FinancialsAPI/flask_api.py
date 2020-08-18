import mysql.connector
from flask import Flask
from flask_restful import Api

my_db = mysql.connector.connect(
    host='localhost',
    user='root',
    password= ,
    database='financials'
)

db_cursor = my_db.cursor()

# store all available financials statements in a list to be used for parameter checking
db_cursor.execute('SHOW TABLES')
tables = db_cursor.fetchall()

financial_statement_list = list()
for table in tables:
    financial_statement_list.append(table[0])

app = Flask(__name__)
api = Api(app)

@app.route('/financials_api/value/<string:accounting_statement>/<string:ticker>/<string:date>/<string:financial_type>', methods=['GET'])
def get_value(accounting_statement, ticker, date, financial_type):
    date_input = date.replace('-', '/')
    # parameter checks:
    # check if the given accounting exists in the database
    if accounting_statement not in financial_statement_list:
        return {'Message': '400 Bad Request. The given financial statement could not be found in the database'}

    # check if the given ticker is valid
    db_cursor.execute('SELECT value FROM {} WHERE ticker="{}"'.format(accounting_statement, ticker))
    available_tickers = db_cursor.fetchall()

    if len(available_tickers) == 0:
        return {'Message': '400 Bad Request. The given ticker could not be found in financial statement. Use only tickers in the DOW30'}

    # check if the given date can be located in the database for the given ticker and return all possible dates
    db_cursor.execute('SELECT date FROM {} WHERE ticker="{}"'.format(accounting_statement, ticker))
    available_dates = db_cursor.fetchall()

    available_dates_list = list()
    for date in available_dates:
        date_to_add = date[0]
        if date_to_add not in available_dates_list:
            available_dates_list.append(date_to_add)

    if date_input not in available_dates_list:
        return {'Message': '400 Bad Request. The given date could not be found in financial statement.',
                'Available dates for given ticker': available_dates_list}

    db_cursor.execute('SELECT financial_type FROM {} WHERE ticker="{}"'.format(accounting_statement, ticker))
    available_financials = db_cursor.fetchall()

    available_financials_list = list()
    for financial in available_financials:
        available_financials_list.append(financial[0])

    if financial_type not in available_financials_list:
        return {'Message': '400 Bad Request. {} was not reported on {} {}.'.format(financial_type, ticker, accounting_statement),
                'Financials on {} {} on {}'.format(ticker, accounting_statement, date): available_financials_list}

    db_cursor.execute('SELECT value FROM {} WHERE ticker="{}" AND date="{}" AND financial_type="{}"'.format(accounting_statement, ticker, date_input, financial_type))
    result = db_cursor.fetchall()

    if result:
        return_dict = {
            'Ticker': ticker,
            'Date': date_input,
            'Financial Type': financial_type,
            'Reported Value': result[0][0]
        }

        return return_dict
    else:
        return {'Message': '400 Bad Request. Check given parameters.'}


@app.route('/financials_api/financial_value/<string:accounting_statement>/<string:ticker>/<string:date>', methods=['GET'])
def get_financials_value(accounting_statement, ticker, date):
    date_input = date.replace('-', '/')
    if accounting_statement not in financial_statement_list:
        return {'Message': '400 Bad Request. The given financial statement could not be found in the database {}'.format(date_input)}

    db_cursor.execute('SELECT value FROM {} WHERE ticker="{}"'.format(accounting_statement, ticker))
    available_tickers = db_cursor.fetchall()

    if len(available_tickers) == 0:
        return {'Message': '400 Bad Request. The given ticker could not be found in financial statement. Use only tickers in the DOW30'}

    db_cursor.execute('SELECT date FROM {} WHERE ticker="{}"'.format(accounting_statement, ticker))
    available_dates = db_cursor.fetchall()

    available_dates_list = list()
    for date in available_dates:
        date_to_add = date[0]
        if date_to_add not in available_dates_list:
            available_dates_list.append(date_to_add)

    if date_input not in available_dates_list:
        return {'Message': '400 Bad Request. The given date could not be found in financial statement.',
                'Available dates for given ticker': available_dates_list}

    db_cursor.execute('SELECT value, financial_type FROM {} WHERE ticker="{}" AND date="{}"'.format(accounting_statement, ticker, date_input))
    result = db_cursor.fetchall()

    if result:
        financial_value = list()
        financial_type = list()

        for key_value in result:
            financial_type.append(key_value[1])
            financial_value.append(key_value[0])

        type_value_info = dict(zip(financial_type, financial_value))

        return_dict = {
            'Ticker': ticker,
            'Date': date_input,
            "Financials": type_value_info
        }

        return return_dict
    else:
        return {'Message': '400 Bad Request. Check given parameters.'}


@app.route('/financials_api/date_value/<string:accounting_statement>/<string:ticker>/<string:financial_type>', methods=['GET'])
def get_financials_date(accounting_statement, ticker, financial_type):
    if accounting_statement not in financial_statement_list:
        return {'Message': '400 Bad Request. The given financial statement could not be found in the database'}

    db_cursor.execute('SELECT value FROM {} WHERE ticker="{}"'.format(accounting_statement, ticker))
    available_tickers = db_cursor.fetchall()

    if len(available_tickers) == 0:
        return {'Message': '400 Bad Request. The given ticker could not be found in financial statement. Use only tickers in the DOW30'}

    db_cursor.execute('SELECT financial_type FROM {} WHERE ticker="{}"'.format(accounting_statement, ticker))
    available_financials = db_cursor.fetchall()

    available_financials_list = list()
    for financial in available_financials:
        available_financials_list.append(financial[0])

    if financial_type not in available_financials_list:
        return {'Message': '400 Bad Request. {} was not reported on {} {}.'.format(financial_type, ticker,
                                                                                   accounting_statement),
                'Financials on {} {}'.format(ticker, accounting_statement): available_financials_list}

    db_cursor.execute('SELECT value, date FROM {} WHERE ticker="{}" AND financial_type="{}"'.format(accounting_statement, ticker, financial_type))
    result = db_cursor.fetchall()

    if result:
        date = []
        financial_value = []

        for key_value in result:
            financial_value.append(key_value[1])
            date.append(key_value[0])

        financial_value_info = dict(zip(financial_value, date))

        return_dict = {
            'Ticker': ticker,
            'Financial_type': financial_type,
            "Financials": financial_value_info
        }

        return return_dict
    else:
        return {'Message': '400 Bad Request. Check given parameters.'}


@app.route('/financials_api/meta_info/<string:accounting_statement>/<string:ticker>')
def get_meta_info(accounting_statement, ticker):
    if accounting_statement not in financial_statement_list:
        return {'Message': '400 Bad Request. The given financial statement could not be found in the database'}

    db_cursor.execute('SELECT value FROM {} WHERE ticker="{}"'.format(accounting_statement, ticker))
    available_tickers = db_cursor.fetchall()

    if len(available_tickers) == 0:
        return {'Message': '400 Bad Request. The given ticker could not be found in financial statement. Use only tickers in the DOW30'}

    db_cursor.execute('SELECT date FROM {} WHERE ticker="{}"'.format(accounting_statement, ticker))
    result = db_cursor.fetchall()

    if result:
        meta_info = dict()

        dates = list()
        for i in range(len(result)):
            if result[i] not in dates:
                dates.append(result[i])

        for date in dates:
            date_as_string = date[0]
            db_cursor.execute('SELECT financial_type, value FROM {} WHERE date="{}"'.format(accounting_statement, date_as_string))
            result = db_cursor.fetchall()

            financial_type = list()
            financial_value = list()

            for key_value in result:
                financial_type.append(key_value[0])
                financial_value.append(key_value[1])

            insert_dict = dict(zip(financial_type, financial_value))
            meta_info[date_as_string] = insert_dict

        return_dict = {
            'Financials': meta_info,
            'Ticker': ticker
        }

        return return_dict
    else:
        return {'Message': '400 Bad Request. Check given parameters.'}


app.run(debug=True)
