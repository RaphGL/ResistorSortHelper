// ### Configuration ###
#define r2 10000		  // value of the R2 resistor
#define BOARD_ITEMS 9	  // last item is is put aside
#define SET_ASIDE_ADDR 10 // address of the set_aside table item
#define V_READ A0		  // the voltage divider value
#define PREV_PAGE 11	  // previous page button adress
#define NEXT_PAGE 12	  // next page button adress
#define SHIFT 2			  // frees up first few ports

// valid resistor values
int resistors[] = {
	6800,
	50,
	120,
	220,
	250,
	560,
	280,
	100,
	1200,
	1500,
	2000,
	2100,
	2500,
	2800,
	3000,
	3500,
	3750,
	4000,
	4100,
	4500,
};

// ### Code ###
// Useful Macros
#define ARR_SIZEOF(X) (sizeof(X) / sizeof((X)[0]))

//  Circuit variables
int prevContainer; // address of the previously selected container
int currPage = 1;
int board[BOARD_ITEMS];
int r1 = 0;				 // resistor to be measured
float voltageRead = 0.0; // voltage divided from r1_r2

// function declarations
void populateBoard(int page);  // populates board with pages with a batch of BOARD_ITEMS
void setPage();				   // a wrapper around populateBoard that ensures that page doesn't go beyond bounds
int itemOnBoard(float val);	   // checks if variable is within the tolerance and on the board[], returns index else -1
void showContainer(int index); // shows where to put resistor
void pushedDown(int addr);	   // wrapper that avoids infinite digitalReads

void setup()
{
	// make all table items as OUTPUT
	for (int i = 0; i < BOARD_ITEMS; i++)
	{
		pinMode(i + SHIFT, OUTPUT);
	}
	pinMode(NEXT_PAGE, INPUT_PULLUP);
	pinMode(PREV_PAGE, INPUT_PULLUP);
	Serial.begin(9600);
}

void loop()
{
	voltageRead = analogRead(V_READ);
	voltageRead *= (5.0 / 1023.0); // converts arduino's 1024 bit 5v mapping back into volts
	if (voltageRead != 0)		   // identify resistor only if value is non-zero
	{
		r1 = (5.0 * r2) / voltageRead - r2; // calculates the value of r1
		setPage();							// set the current page of resistors
		showContainer(itemOnBoard(r1));		// point to that resistor's container
	}
	else
	{
		Serial.println("No resistor placed in.");
	}

	Serial.println(currPage);
	delay(200);
}

int itemOnBoard(float val)
{
	for (int i = 0; i < BOARD_ITEMS; i++)
	{
		int min_val = board[i] * 0.90; // minimum tolerance
		int max_val = board[i] * 1.10; // maximum tolerance

		if (val > min_val && val < max_val) // is val within tolerance?
		{
			return i + SHIFT; // returns the index of the value that meets the criteria
		}
	}
	return -1;
}

void showContainer(int index)
{
	digitalWrite(prevContainer, LOW); // turn off previous measurement's LED
	if (index == -1)				  // if resistor not in page, set it aside
	{
		digitalWrite(SET_ASIDE_ADDR, HIGH);
		prevContainer = SET_ASIDE_ADDR;
	}
	else // if in page, show its container
	{
		digitalWrite(index, HIGH);
		prevContainer = index;
	}
}

void populateBoard(int page)
{
	int curr_index = 0; // stores the calculated index for the resistors items
	// add all resistors in current page to the board
	for (int i = 0; i < BOARD_ITEMS; i++)
	{
		curr_index = BOARD_ITEMS * (page - 1) + i;
		board[i] = resistors[curr_index];
	}
}

void setPage()
{
	float numOfPages = ARR_SIZEOF(resistors) / float(BOARD_ITEMS);

	// only increment if currPage doesn't go over the total number of pages
	if (digitalRead(NEXT_PAGE) == HIGH && currPage <= numOfPages)
	{
		currPage++;
	}

	// only decrement if currPage doesn't go below zero
	if (digitalRead(PREV_PAGE) == HIGH && currPage > 0)
	{
		currPage--;
	}

	populateBoard(currPage);
}
