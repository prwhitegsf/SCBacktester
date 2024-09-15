# SCBacktester

SCBacktester is a study for the [Sierra Chart](https://www.sierrachart.com/index.php?page=doc/ACSIL_Members_Functions.html) trading platform that allows testing of automated strategy performance on historical data. 

The study takes the following inputs from the user's strategy: 
- open contracts
- entry price (optional)
- exit price (optional)

Then displays/outputs the results of the strategy via:
- Sierra Chart spreadsheet 
- Study Subgraph (for on chart visualization)
- CSV files (optional)

Aggregated strategy results are displayed via a Sierra Chart Spreadsheet:

![table with strategy results](/img/Table.jpg)

The study also collects the details of each trade on another sheet:

![table with strategy results](/img/TradesList.jpg)

Strategy performance can also be viewed on-chart via SCBacktester's subgraph outputs:

![chart with equity curve](/img/chart.jpg)

## Usage
### Building the study
- Have a look at this [Sierra Chart Guide](https://www.sierrachart.com/index.php?page=doc/HowToBuildAnAdvancedCustomStudyFromSourceCode.html)
- Last I checked you could upload up to 20 files to compile remotely on Sierra's servers. This project has currenty has 19 files, so it's possible. In the future, I'll probably assemble a version that fits into a 2-3 files to make that process easier.
- If you're building locally:
  * include the Sierra Chart source directory: C:\SierraChart\ACS_Source
  * project requires C++20
  * output DLL should be placed in: C:\SierraChart\Data\


### Working with SCBacktester

![input window](/img/Inputs.jpg)

#### Open Contracts

At the minimum, testing your strategy requries your strategy study have a subgraph that outputs whether or not a position is open at a given time (typically on each chart bar / interval). 
- if the strategy has 0 open contracts / shares
  * the subgraph should output 0
- if the strategy is long
  * the subgraph should output 1 or a positive number representing the number of open contracts / shares (ie if the strategy trades 10 contracts at a time, this value would be 10 whenever there's an open long position)
- if the strategy is short
  * the subgraph should output -1 or a negative number representing the number of open contracts / shares (ie if the strategy trades 10 contracts at a time, this value would be -10 whenever there's an open short position)
- Note SCBacktester does not currently provide support for scaling in and out of positions.

![open contracts output](/img/OpenContracts.jpg)

#### Entry / Exit Prices
Additionally SCBacktester allows for input of strategy entry and exit prices. These values are recorded ON THE BAR the trade closes. 

Optionally, you can have SCBacktester calculate entry/exit prices based on the closes on the first bar of the open position and the last bar of the open position respectively.

#### CSV Output
SCBacktester allows for otputting the trade list and the strategy stats table to a csv file. To enable this you need to add an [Advanced Custom Study Control Bar Button](https://www.sierrachart.com/index.php?page=doc/ControlBar.php) to your control bar and set the SCBacktester input "WriteTradesButton" to the button number you selected. With the button engaged perform a chart recalculation (ctrl + insert) to write the csv.

Currently, a folder structure is created inside the SierraChart\Data folder for the exported files. In the future there will be a user input to select the directory to write the csv files.

## Code Structure

For each chart bar, SCBacktester checks for a closed trade:
- if there was an open position on the previous chart bar
- if the number of open contracts on the current chart bar is different from the previous bar
- then SCBacktester records the trade details

Trade / TradeList / EquityCurve 
- Trade
  * each time a closed trade is detected, its details are written to a trade object. The trade object holds all the basic information about the trade: entry / exit prices, entry /exit times, profit / loss, MAE / MFE, etc.
  * All prices are converted from floating point values to their equivelent integer price-in-ticks. This is necessary to avoid floating point calculation inconsistencies and allows the study to function on trading instruments with fractional prices (ie, futures contracts on treasuries). They are converted back to floating point values on output.
- TradeList
  * a unique pointer is assigned to each trade object and pushed to a vector holding all the trades.
- EquityCurve
  * gets info from the trade list to calculate and output / display
    * Cumulative Profit/Loss
    * Max Drawdown
    * Equity Peak / Valley
  * While the above metrics are calculated only from closed trades, it's often useful to look at intratrade risk
    * Intratrade Equity Peak / Valley
    * Intratrafr Max Drawdown

StrategyStats
- On the 2nd to last bar of the chart (the last bar guaranteed to be closed) we calculate the results of the trading strategy. The StrategyStats class takes a reference to the trade_list as input and aggregates the performance metrics.

SpreadsheetWriter / CsvWriter / FolderCreator
- SpreadsheetWriter
  * takes a reference to strategy stats and outputs the appropriate values to the SierraChart spreadsheet.
- CsvWriter
  * takes a reference to strategy stats and outputs the trade list to a csv file.

Pointer Manager

- SCBacktester uses ACSIL's [persistent pointer function](https://www.sierrachart.com/index.php?page=doc/ACSIL_Members_Functions.html#PersistentVariableFunctions) to maintain trade data between function calls. The pointer manager class provides functions for initialization, null checks and releasing memory.

