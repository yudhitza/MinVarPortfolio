# Minimal Variance Portfolio Project

*Group Project by Hu Chen Nan, Francheska Orellana, Alina Armijo, Helesa Lahey*

  First, our program downloads trading price data for S&P 500 stocks, writes it to the files in a folder, and stores closing prices for 3 years (750 days) in a vector for those stocks. Based on the close prices, our program calculates daily return for each trading day and the average daily return over the time period. Our program next calculates the covariance matrix. To calculate the minimal variance portfolio, our program has functions that will multiply two matrices and invert a matrix using LU decomposition. Using the data and functions described above, our program calculates the minimum variance portfolio according to required formulas and outputs the required weights for each stock.
  
  ## Notes ##
  * This was for a class project 
  * There are different ways to implement this, i.e. classes for matrices
  * Instead of outputing the weights for all stocks that the program can find for, the program can be coded so that the user enters the stocks they what the weights for and have the code output the weights for *only those* stocks
  * Downloading the information from the S&P 500 takes about 15-20 minutes
  
