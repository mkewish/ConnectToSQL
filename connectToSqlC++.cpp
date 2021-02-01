//
//  connectToSqlC++.cpp
//  
//
//  Created by Makoto Kewish on 11/26/20.
//
//---------------------------------------------------------------------------
// Author: Makoto Kewish
// Class: CPSC321
// Assignment: HW8
// File: connectToSqlC++.cpp
// Description: This program accesses and modifies a mysql database using c++
//----------------------------------------------------------------------------

#include <iostream>
#include <mysql_connection.h>
#include <driver.h>
#include <prepared_statement.h>
#include <resultset.h>
#include <exception.h>
#include "connectToSqlC++.hpp"


using namespace std;

string to_upper(const string& s);

void displayMainMenu(sql::Connection *con);
void listCountries(sql:: Connection *con);
void addCountry(sql:: Connection *con);
void findCountryGdp(sql:: Connection *con);
void updateCountryGdp(sql:: Connection *con);

int main() {
  try {
    sql::Driver *driver = get_driver_instance();
    sql::Connection *con = driver -> connect (HOST, USER, PASS);
    string db = string(USER) + "_DB";
    con -> setSchema(db);

    displayMainMenu(con);
    cout << "Exit" << endl;
  }
  catch (sql::SQLException &e) {
    cout << e.what() << endl;
  }

  return 0;
}

/*
  Displays the main menu. User has the option to list countries, add a country
  find countries based on a country's gdp and inflation, update a country's
  gdp and inflation, and exit the program.
                                                                
  Parameters: sql::Connection *con
*/
void displayMainMenu(sql::Connection *con) {
    char choice;
    do {
        cout << "Main Menu:" << endl;
        cout << "1. List countries" << endl;
        cout << "2. Add country" << endl;
        cout << "3. Find countries based on gdp and inflation" << endl;
        cout << "4. Update country's gdp and inflation" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice (1-5): ";
        cin >> choice;
        
        cout << endl;
        switch (choice) {
        case '1':
            listCountries(con);
            break;
        case '2':
            addCountry(con);
            break;
        case '3':
            findCountryGdp(con);
            break;
        case '4':
            updateCountryGdp(con);
            break;
        case '5':
            return;
        default:
            cout << "Invalid input...\n" << endl;
            choice = '0';
        }
    } while (choice == '0');

}
/*
 Lists all the countries (name and code) in the database

 Parameters: sql::Connection *con
*/
void listCountries(sql:: Connection *con) {
    try {
        sql::Statement *stmt = con -> createStatement();
        string getCountry = "SELECT country_name, country_code FROM Country";
        sql::ResultSet *res = stmt -> executeQuery(getCountry);

        cout << "All countries in database..." << endl;
        while (res -> next()) {
            string country = res -> getString("country_name");
            string code = res -> getString("country_code");
            cout << country << " (" << code << ")" << endl;
        }
        cout << endl;

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }

    displayMainMenu(con);
}

/*
  Add a country if doesnt exist in database
                                                                
  Parameters: sql::Connections *con
 */
void addCountry(sql:: Connection *con) {
    cout << "Add Country..." << endl;
    string country_name;
    string country_code;
    string gdp;
    string inflation;

    cout << "Country code................: ";
    cin >> country_code;
    cin.ignore();
    cout << "Country name................: ";
    getline(cin, country_name);
    cout << "Country per capita gdp (USD): ";
    cin >> gdp;
    cin.ignore();
    cout << "Country inflation (pct).....: ";
    cin >> inflation;
    cin.ignore();

    try {
        string check = "SELECT COUNT(*) FROM Country WHERE country_code = ?";
        sql::PreparedStatement *prep_stmt = con -> prepareStatement(check);
        prep_stmt -> setString(1, country_code);
        sql::ResultSet *check_res = prep_stmt -> executeQuery();
        check_res -> next();
      
        if(check_res -> getInt("COUNT(*)") >= 1) {
            cout << "\nCountry code already exists...\n" << endl;
        } else {
            delete prep_stmt;
            string insert = "INSERT INTO Country VALUES (?,?,?,?)";
            prep_stmt = con -> prepareStatement(insert);
            prep_stmt -> setString(1, country_code);
            prep_stmt -> setString(2, country_name);
            prep_stmt -> setString(3, gdp);
            prep_stmt -> setString(4, inflation);

            prep_stmt -> executeUpdate();
            cout << "\nSuccessfully inserted into database!\n" << endl;
        }
        
        delete prep_stmt;
        delete check_res;
    }
    catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }

    displayMainMenu(con);
}

/*
  Find countries based on its GDP and inflation.
                                                                        
  Parameters: sql::Connection *con
 */
void findCountryGdp(sql:: Connection *con) {
    cout << "Find country's GDP and Inflation..." << endl;
    string  numCountries;
    string minGdpPerCapita;
    string maxInflation;

    cout << "Number of countries to display: ";
    cin >> numCountries;
    cout << "Minimum per capita gdp (USD)..: ";
    cin >> minGdpPerCapita;
    cout << "Maximum inflation (pct).......: ";
    cin >> maxInflation;
    cout << endl;

    try {
      string find = "SELECT country_name, country_code, gdp, inflation FROM Country WHERE gdp >= ? AND i\
nflation <= ? ORDER BY gdp DESC LIMIT ?";
      sql::PreparedStatement *prep_stmt = con -> prepareStatement(find);
      prep_stmt -> setString(1, minGdpPerCapita);
      prep_stmt -> setString(2, maxInflation);
      prep_stmt -> setString(3, numCountries);
      sql::ResultSet *find_res = prep_stmt -> executeQuery();
      
      while(find_res -> next()) {
        string country = find_res -> getString("country_name");
        string code = find_res -> getString("country_code");
        
        string gdp = find_res -> getString("gdp");
        string inflation = find_res -> getString("inflation");
        cout << country << " (" << code << "), " << gdp << ", " << inflation << endl;
    }
    cout << endl;
    
    delete prep_stmt;
    delete find_res;
    }
    catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }
    displayMainMenu(con);
}

/*
  Updates a country's GDP and inflation
                                                                        
  Parameters: sql::Connection *con
 */
void updateCountryGdp(sql:: Connection *con) {
    cout << "Update country's GDP and Inflation..." << endl;
    string country_code;
    string gdp;
    string inflation;

    cout << "Country code................: ";
    cin >> country_code;
    cout << "Country per capita gdp (USD): ";
    cin >> gdp;
    cout << "Country inflation (pct).....: ";
    cin >> inflation;

    try {
        string check = "SELECT COUNT(*) FROM Country WHERE country_code = ?";
        sql::PreparedStatement *prep_stmt = con -> prepareStatement(check);
        prep_stmt -> setString(1, country_code);
        sql::ResultSet *check_res = prep_stmt -> executeQuery();
        check_res -> next();

        if(check_res -> getInt("COUNT(*)") == 0) {
            cout << "Country does not exist..." << endl;
        } else {
            delete prep_stmt;
            string update = "UPDATE Country SET gdp = ?, inflation = ? WHERE country_code = ?";
            prep_stmt = con -> prepareStatement(update);
            prep_stmt -> setString(1, gdp);
            prep_stmt -> setString(2, inflation);
            prep_stmt -> setString(3, country_code);

            prep_stmt -> executeUpdate();
            cout << "\nSuccessfully updated column!\n" << endl;
        }
        
        delete prep_stmt;
        delete check_res;
    }
    catch (sql::SQLException &e) {
        cout << e.what() << endl;
    }

    displayMainMenu(con);
}

