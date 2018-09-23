/*
 * file_dialog.hpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#ifndef SRC_FILE_DIALOG_HPP_
#define SRC_FILE_DIALOG_HPP_

#include <string>

#include "app.hpp"
#include "pane.hpp"
#include "menu.hpp"
#include "line_editor.hpp"

namespace textx {
	using namespace std;
	
	typedef void (*FileDialogHandler)(bool ok, string path, void* cookie);
	
	class FileDialogApp : public App {
	private:
		MenuBar menuBar;
		LineEditor lineEditor;
	public:
		FileDialogHandler handler;
		void* cookie;
		
		FileDialogApp(Pane* pane, string suggested, FileDialogHandler handler, void* cookie);
		
		// overrides
		void refresh();
		void onKey(curses::KeyCode key);
		string getTitle();
		MenuBar* getMenuBar();
	};
}

#endif /* SRC_FILE_DIALOG_HPP_ */
