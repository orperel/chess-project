#include "ChessGuiCommons.h"
#include "Chess.h"
#include "ChessGuiGameControl.h"
#include "Types.h"
#include "GameCommands.h"

/** Creates a dialog, with a dynamic number of buttons, under the given window, and using the buttonImagePath as
 *  a base for all images. All images are expected to end with 1, 2, etc (the index of the next image).
 *	Images are expected to exist for all numOfButtons values.
 *  "values" is the sequence of values for each dialog option (preallocated dynamic array).
 */
GuiDialog* createDyanmicDialog(GuiWindow* window, int numOfButtons, const char* buttonImgPath, int* values)
{
	GuiDialog* dialog = createDialog(window, BUTTON_W, BUTTON_H, DIALOG_BGIMAGE, GREEN, BLACK);
	if ((NULL == dialog) || (g_guiError))
	{
		return NULL;
	}

	// Allocate space for an image path string
	int fileImgLength = strlen(buttonImgPath) + strlen(RESOURCE_IMG_EXT) + 2;
	char* fileImg = (char*)malloc(sizeof(char)*fileImgLength);
	if (NULL == fileImg)
	{
		g_memError = true;
		return NULL;
	}

	// Create number of buttons buttons in a generic way
	char slotStr[2];
	int i;
	for (i = 1; i <= numOfButtons; i++)
	{
		// Init the string with the path of the next button image
		// It is okay to override the previous string since the image of the previous button should have been loaded
		// synchronically already.
		strcpy(fileImg, buttonImgPath);
		sprintf(slotStr, "%d", i);
		strcat(fileImg, slotStr);
		strcat(fileImg, RESOURCE_IMG_EXT);

		dialog->addOption(dialog, fileImg, MAGENTA, &values[i - 1]);
		if (g_guiError)
		{
			g_guiError = true;
			return NULL;
		}
	}

	free(fileImg);

	return dialog;
}

/** Show the "depth" dialog, for choosing a depth for minmax.
 *	The depth chosen by the user is returned.
 *	DIFFICULTY_BEST_INT is returned if BEST is chosen.
 *	(DIFFICULTY_BEST_INT - 1) is returned if the dialog is canceled.
 *	(DIFFICULTY_BEST_INT - 2) is returned on error.
 */
int showDepthDialog(GuiWindow* window)
{
	int depth;

	// Initialize the dialog values
	int* values = (int*)malloc(sizeof(int)*MAX_DEPTH);
	if (NULL == values)
	{
		g_memError = true;
		return (DIFFICULTY_BEST_INT - 2);
	}
	int i;
	for (i = 1; i <= MAX_DEPTH; i++)
		values[i - 1] = i;

	GuiDialog* dialog = createDyanmicDialog(window, MAX_DEPTH, MINMAX_DEPTH_IMG_PATH, values);
	if ((g_guiError) || (g_memError) || (NULL == dialog))
	{
		return (DIFFICULTY_BEST_INT - 2);
	}

	int bestValue = DIFFICULTY_BEST_INT;
	int cancelValue = DIFFICULTY_BEST_INT - 1;
	dialog->addOption(dialog, MINMAX_BEST_DEPTH_IMG_PATH, MAGENTA, &bestValue);
	if (g_guiError)
	{
		return (DIFFICULTY_BEST_INT - 2);
	}
	dialog->addOption(dialog, BUTTON_CANCEL_IMG, MAGENTA, &cancelValue);
	if (g_guiError)
	{
		return (DIFFICULTY_BEST_INT - 2);
	}

	// Show the dialog and get the results the user have chosen
	depth = *((int*)dialog->showDialog(dialog));
	free(values);
	if ((g_guiError) || (g_memError))
	{
		return (DIFFICULTY_BEST_INT - 2);
	}

	return depth;
}

/** Open the save / load game to slots dialog.
 *	The save slot path chosen by the user is returned (and should be freed by the caller).
 *	NULL is returned if the dialog is canceled or an error occured (for errors, global flags are set).
 */
char* showLoadSaveDialog(GuiWindow* window)
{
	// Initialize the dialog values
	int* values = (int*)malloc(sizeof(int)*NUM_OF_SAVE_SLOTS);
	if (NULL == values)
	{
		g_memError = true;
		return NULL;
	}
	int i;
	for (i = 1; i <= NUM_OF_SAVE_SLOTS; i++)
		values[i - 1] = i;

	GuiDialog* dialog = createDyanmicDialog(window, NUM_OF_SAVE_SLOTS, SAVE_SLOT_PATH, values);
	if ((g_guiError) || (g_memError) || (NULL == dialog))
	{
		free(values);
		return NULL;
	}

	int cancelValue = -1; // This value is't configurable, but we know it must be lower than 0
						  // so we save it to a local variable and don't use a constant
	dialog->addOption(dialog, BUTTON_CANCEL_IMG, MAGENTA, &cancelValue);
	if (g_guiError)
	{
		free(values);
		return NULL;
	}

	// Show the dialog and get the results the user have chosen
	int slotNum = *((int*)dialog->showDialog(dialog));
	free(values);
	if ((g_guiError) || (g_memError) || (slotNum == cancelValue))
	{
		return NULL;
	}

	// Save the file to the disk. We use a predetermined file path for each slot
	char slotStr[2];
	int saveFileLength = strlen(SAVE_GAME_PATH) + strlen(SAVE_FILE_EXTENSION) + 2;
	char* saveFilePath = (char*)malloc(sizeof(char)*saveFileLength);
	if (NULL == saveFilePath)
		return NULL;
	strcpy(saveFilePath, SAVE_GAME_PATH);
	sprintf(slotStr, "%d", slotNum);
	strcat(saveFilePath, slotStr);
	strcat(saveFilePath, SAVE_FILE_EXTENSION);

	return saveFilePath;
}