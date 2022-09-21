﻿/*
noMeiryoUI (C) 2005,2012-2022 Tatsuhiko Shoji
The sources for noMeiryoUI are distributed under the MIT open source license
*/
#pragma warning(disable : 4996)

#include "util.h"
#include "iniReader.h"

/** Windows 8のフォントサイズ算出式を使用するか？ */
bool WIN8_SIZE = true;
/** 言語リソース */
std::vector<tstring> langResource;

/** フォント名(Windows 8.x) */
std::vector<tstring> fontFaces8;
/** フォントサイズ(Windows 8.x) */
std::vector<int> fontSizes8;
/** フォント文字セット(Windows 8.x) */
std::vector<int> fontCharset8;

/** フォント名(Windows 10) */
std::vector<tstring> fontFaces10;
/** フォントサイズ(Windows 10) */
std::vector<int> fontSizes10;
/** フォント文字セット(Windows 10) */
std::vector<int> fontCharset10;

/** フォント名(Windows 11) */
std::vector<tstring> fontFaces11;
/** フォントサイズ(Windows 11) */
std::vector<int> fontSizes11;
/** フォント文字セット(Windows 11) */
std::vector<int> fontCharset11;

int codePage = 0;
bool isKorean = false;

/**
 * フォントのピクセル数に対応するポイント数を整数で算出する。(Windows 8)
 *
 * @param font フォント情報
 * @param hWnd ウインドウハンドル
 * @return フォントサイズ
 */
int getFontPointInt(LOGFONT *font, HWND hWnd)
{
	double point = getFontPoint(font, hWnd);

	if (WIN8_SIZE) {
		// Windows 8ディスプレイコントロールパネル互換
		if ((point > 10) || (point < 8)) {
			return (int)point;
		} else {
			// 10ptまではWindows 7と同様に計算する。
			// Windows 7以前互換 
			if (point - abs((int)point) > 0.49) {
				return (int)point + 1;
			} else {
				return (int)point;
			}
		}
	} else {
		// Windows 7以前互換 
		if (point - abs((int)point) > 0.49) {
			return (int)point + 1;
		} else {
			return (int)point;
		}
	}
}

/**
 * フォントのピクセル数に対応するポイント数を整数で算出する。(Windows 8)
 *
 * @param font フォント情報
 * @param hWnd ウインドウハンドル
 * @return フォントサイズ
 */
double getFontPointDouble(LOGFONT* font, HWND hWnd)
{
	double point = getFontPoint(font, hWnd);

	if (WIN8_SIZE) {
		// Windows 8ディスプレイコントロールパネル互換
		if ((point > 10) || (point < 8)) {
			return point;
		}
		else {
			// 10ptまではWindows 7と同様に計算する。
			// Windows 7以前互換 
			if (point - abs((int)point) > 0.49) {
				return point + 1;
			}
			else {
				return point;
			}
		}
	}
	else {
		// Windows 7以前互換 
		if (point - abs((int)point) > 0.49) {
			return point + 1;
		}
		else {
			return point;
		}
	}
}

/**
 * フォントのピクセル数に対応するポイント数を算出する。
 * (Windows 7/コモンダイアログ互換)
 *
 * @param font フォント情報
 * @param hWnd ウインドウハンドル
 * @return フォントサイズ
 */
double getFontPoint(LOGFONT *font, HWND hWnd)
{
	// フォントを作成する。
	HFONT hFont = CreateFontIndirect(font);
	// 自身のウインドウハンドルから作成したデバイスコンテキストに
	// フォントを設定する。
	HDC dc = GetDC(hWnd);
	SelectFont(dc, hFont);

	// デバイスコンテキストからTEXTMETRICを取得する。
	TEXTMETRIC metric;
	GetTextMetrics(dc, &metric);

	int logPixelY = GetDeviceCaps(dc, LOGPIXELSY);

	ReleaseDC(hWnd, dc);
	DeleteObject(hFont);

	int height;
	if (font->lfHeight < 0) {
		// 負の場合はlfHeightはフォント自体の高さ。
		height = 0 - font->lfHeight;
	} else if (font->lfHeight > 0) {
		// 正の場合はすでにInternal Leadingを含んでいるのでその分を引く。
		height = font->lfHeight - metric.tmInternalLeading;
	} else {
		// 0の場合はデフォルトの大きさを作成したフォントから取得する。
		height = metric.tmAscent + metric.tmDescent - metric.tmInternalLeading;
	}

	double point = (double)height * 72 / logPixelY;

	return point;
}

/**
 * リソースの読み込みを行う。
 *
 * @param file リソースファイル名
 * @param key キー名
 * @param 読み込めなかった時の文字列
 */
void readResourceItem(TCHAR *file, TCHAR *key, TCHAR *fallback)
{
	tstring buf;
	int len;

	// INIファイルを読み込む。Unicode版のAPIでもファイルが非Unicodeの場合は
	// 各言語の文字コードのファイルとして読んでくれる。
	len = GetPrivateProfileStringExT("RESOURCE", key, _T(""), buf, file);

	if (len > 0) {
		langResource.push_back(buf);
	} else {
		langResource.push_back(fallback);
	}

}

/**
 * リソースの読み込みを開始する。
 *
 * @param file リソースファイル名
 */
void readResourceFile(TCHAR *file)
{
	readResourceItem(file, _T("FONT_FACE"),
		_T("System")
	);
	readResourceItem(file, _T("TITLE"),
		_T("No!! Meiryo UI")
	);
	readResourceItem(file, _T("MENU_FILE"),
		_T("&File")
	);
	readResourceItem(file, _T("MENU_FILE_LOAD"),
		_T("L&oad font settings...")
	);
	readResourceItem(file, _T("MENU_FILE_SAVE"),
		_T("S&ave font settings...")
	);
	readResourceItem(file, _T("MENU_FILE_SET_QUIT"),
		_T("&Set system fonts and quit")
	);
	readResourceItem(file, _T("MENU_FILE_QUIT"),
		_T("E&xit")
	);
	readResourceItem(file, _T("MENU_PRESET"),
		_T("&Preset")
	);
	readResourceItem(file, _T("MENU_PRESET_8"),
		_T("Windows 8/8.1")
	);
	readResourceItem(file, _T("MENU_PRESET_10"),
		_T("Windows 10")
	);
	readResourceItem(file, _T("MENU_TOOLS"),
		_T("&Tools")
	);
	readResourceItem(file, _T("MENU_TOOLS_THREAD"),
		_T("Set system fonts in separated thread")
	);
	readResourceItem(file, _T("MENU_TOOLS_SEVEN"),
		_T("Calculate font size same as Windows 7")
	);
	readResourceItem(file, _T("MENU_HELP"),
		_T("&Help")
	);
	readResourceItem(file, _T("MENU_HELP_HELP"),
		_T("&Help")
	);
	readResourceItem(file, _T("MENU_HELP_ABOUT"),
		_T("&About No!! Meiryo UI")
	);
	readResourceItem(file, _T("DLG_ALL_FONT"),
		_T("All fonts")
	);
	readResourceItem(file, _T("DLG_TITLE_BAR"),
		_T("Title bar")
	);
	readResourceItem(file, _T("DLG_ICON"),
		_T("Icon")
	);
	readResourceItem(file, _T("DLG_PALETTE"),
		_T("Palette title")
	);
	readResourceItem(file, _T("DLG_HINT"),
		_T("Hint")
	);
	readResourceItem(file, _T("DLG_MESSAGE"),
		_T("Message box")
	);
	readResourceItem(file, _T("DLG_MENU"),
		_T("Menu")
	);
	readResourceItem(file, _T("DLG_SELECT"),
		_T("Select...")
	);
	readResourceItem(file, _T("DLG_SET_ALL"),
		_T("Set all")
	);
	readResourceItem(file, _T("DLG_SET"),
		_T("Set")
	);
	readResourceItem(file, _T("DLG_EXIT"),
		_T("Exit")
	);
	readResourceItem(file, _T("DLG_FONT_SEL"),
		_T("Font Selection")
	);
	readResourceItem(file, _T("DLG_FONT_NAME"),
		_T("&Font name")
	);
	readResourceItem(file, _T("DLG_STYLE"),
		_T("St&yle")
	);
	readResourceItem(file, _T("DLG_SIZE"),
		_T("&Size")
	);
	readResourceItem(file, _T("DLG_UNDERLINE"),
		_T("&Under line")
	);
	readResourceItem(file, _T("DLG_STRIKE"),
		_T("S&trikethrough")
	);
	readResourceItem(file, _T("DLG_CHARSET"),
		_T("&Character set")
	);
	readResourceItem(file, _T("DLG_OK"),
		_T("OK")
	);
	readResourceItem(file, _T("DLG_CANCEL"),
		_T("Cancel")
	);

	readResourceItem(file, _T("DLG_CHARSET_ANSI"),
		_T("Ansi")
	);
	readResourceItem(file, _T("DLG_CHARSET_BALTIC"),
		_T("Baltic")
	);
	readResourceItem(file, _T("DLG_CHARSET_BIG5"),
		_T("Traditional Chinese")
	);
	readResourceItem(file, _T("DLG_CHARSET_DEFAULT"),
		_T("Default")
	);
	readResourceItem(file, _T("DLG_CHARSET_EASTEUROPE"),
		_T("East Europe")
	);
	readResourceItem(file, _T("DLG_CHARSET_GB2312"),
		_T("Simplified Chinese")
	);
	readResourceItem(file, _T("DLG_CHARSET_GREEK"),
		_T("Greek")
	);
	readResourceItem(file, _T("DLG_CHARSET_HANGUL"),
		_T("Hangul")
	);
	readResourceItem(file, _T("DLG_CHARSET_MAC"),
		_T("Mac")
	);
	readResourceItem(file, _T("DLG_CHARSET_OEM"),
		_T("OEM")
	);
	readResourceItem(file, _T("DLG_CHARSET_RUSSIAN"),
		_T("Russian")
	);
	readResourceItem(file, _T("DLG_CHARSET_SHIFTJIS"),
		_T("Japanese")
	);
	readResourceItem(file, _T("DLG_CHARSET_SYMBOL"),
		_T("Symbol")
	);
	readResourceItem(file, _T("DLG_CHARSET_TURKISH"),
		_T("Turkish")
	);
	readResourceItem(file, _T("DLG_CHARSET_VIETNAMESE"),
		_T("Vietnamese")
	);
	readResourceItem(file, _T("DLG_CHARSET_JOHAB"),
		_T("Johab Korean")
	);
	readResourceItem(file, _T("DLG_CHARSET_ARABIC"),
		_T("Arabic")
	);
	readResourceItem(file, _T("DLG_CHARSET_HEBREW"),
		_T("Hebrew")
	);
	readResourceItem(file, _T("DLG_CHARSET_THAI"),
		_T("Thai")
	);

	readResourceItem(file, _T("DLG_STYLE_NORMAL"),
		_T("Regular")
	);
	readResourceItem(file, _T("DLG_STYLE_ITALIC"),
		_T("Italic")
	);
	readResourceItem(file, _T("DLG_STYLE_BOLD"),
		_T("Bold")
	);
	readResourceItem(file, _T("DLG_STYLE_BOLD_ITALIC"),
		_T("Bold Italic")
	);

	readResourceItem(file, _T("MSG_SETTING_FILE"),
		_T("Font settings file(*.ini)")
	);
	readResourceItem(file, _T("MSG_ALL_FILE"),
		_T("All file(*.*)")
	);
	readResourceItem(file, _T("MSG_LOAD_FAIL"),
		_T("Font settings file load failed.")
	);
	readResourceItem(file, _T("MSG_SAVE_FAIL"),
		_T("Font settings file save failed.")
	);
	readResourceItem(file, _T("MSG_ERROR"),
		_T("Error")
	);
	readResourceItem(file, _T("MSG_ABOUT"),
		_T("About No!! Meiryo UI")
	);
	readResourceItem(file, _T("MSG_NO_FONT"),
		_T("Please select font.")
	);
	readResourceItem(file, _T("MSG_NO_STYLE"),
		_T("Please select style.")
	);
	readResourceItem(file, _T("MSG_NO_SIZE"),
		_T("Please select size.")
	);
	readResourceItem(file, _T("MSG_NO_CHARSET"),
		_T("Please select charset.")
	);
	readResourceItem(file, _T("MSG_TRANSLATE"),
		_T("Tatsuhiko Shoji(Tatsu)")
	);
	readResourceItem(file, _T("FONT_CHARSET"),
		_T("1")
	);

	// 71
	readResourceItem(file, _T("DLG_APPLY"),
		_T("Apply")
	);
	readResourceItem(file, _T("DLG_ALL_GROUP"),
		_T("All font setting")
	);
	readResourceItem(file, _T("DLG_INDIVIDUAL_GROUP"),
		_T("Individual font setting")
	);

	readResourceItem(file, _T("DLG_STYLE_MEDIUM"),
		_T("Medium")
	);
	readResourceItem(file, _T("DLG_STYLE_SEMI_BOLD"),
		_T("Semibold")
	);
	readResourceItem(file, _T("DLG_STYLE_BOOK"),
		_T("Book")
	);
	readResourceItem(file, _T("DLG_STYLE_OBLIQUE"),
		_T("Oblique")
	);
	readResourceItem(file, _T("DLG_STYLE_LIGHT"),
		_T("Light")
	);
	readResourceItem(file, _T("DLG_STYLE_EXTRA_LIGHT"),
		_T("ExtraLight")
	);
	readResourceItem(file, _T("DLG_STYLE_SEMI_LIGHT"),
		_T("SemiLight")
	);
	readResourceItem(file, _T("DLG_STYLE_SLANTED"),
		_T("Slanted")
	);
	readResourceItem(file, _T("DLG_CHECK_BOLD"),
		_T("&Bold")
	);
	readResourceItem(file, _T("DLG_CHECK_ITALIC"),
		_T("&Italic")
	);
	readResourceItem(file, _T("MSG_WARNING"),
		_T("Warning")
	);
	readResourceItem(file, _T("MSG_WIN11_22H2RESTRICTION"),
		_T("On Windows 11 2022 Update due to Windows reason title bar font can't change.")
	);
}

/**
 * リソースの読み込みを行う(フォント名用)。
 *
 * @param buffer 格納先
 * @param file リソースファイル名
 * @param key キー名
 */
int readFontFace(std::vector<tstring> &buffer, TCHAR *file, TCHAR *key)
{
	tstring buf;
	int len;

	// INIファイルを読み込む。
	len = GetPrivateProfileStringExT(PRESET_SECTION, key, _T(""), buf, file);

	buffer.push_back(buf);

	return len;
}

/**
 * リソースの読み込みを行う(フォントサイズ用)。
 *
 * @param buffer 格納先
 * @param file リソースファイル名
 * @param key キー名
 */
int readFontSize(std::vector<int> &buffer, TCHAR *file, TCHAR *key)
{
	int size;

	// INIファイルを読み込む。Unicode版のAPIでもファイルが非Unicodeの場合は
	// 各言語の文字コードのファイルとして読んでくれる。
	size = GetPrivateProfileInt(_T(PRESET_SECTION), key, 0, file);
	buffer.push_back(size);

	return size;
}

/**
 * リソースの読み込みを行う(フォント文字セット用)。
 *
 * @param buffer 格納先
 * @param file リソースファイル名
 * @param key キー名
 */
int readFontCharset(std::vector<int> &buffer, TCHAR *file, TCHAR *key)
{
	int size;

	// INIファイルを読み込む。Unicode版のAPIでもファイルが非Unicodeの場合は
	// 各言語の文字コードのファイルとして読んでくれる。
	size = GetPrivateProfileInt(_T(PRESET_SECTION), key, 1, file);
	buffer.push_back(size);

	return size;
}

/**
 * Windows 8のフォントプリセットリソースの読み込みを行う
 *
 * @param file リソースファイル名
 * @return 0:設定失敗 1:設定成功
 */
int readFontResource8(TCHAR *file)
{
	int result;

	// フォント名
	result = readFontFace(fontFaces8, file, _T("CAPTION_FACE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces8, file, _T("ICON_FACE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces8, file, _T("SMALLCAPTION_FACE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces8, file, _T("STATUS_FACE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces8, file, _T("MESSAGE_FACE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces8, file, _T("MENU_FACE_8"));
	if (result == 0) {
		return 0;
	}

	// 文字サイズ
	result = readFontSize(fontSizes8, file, _T("CAPTION_SIZE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes8, file, _T("ICON_SIZE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes8, file, _T("SMALLCAPTION_SIZE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes8, file, _T("STATUS_SIZE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes8, file, _T("MESSAGE_SIZE_8"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes8, file, _T("MENU_SIZE_8"));
	if (result == 0) {
		return 0;
	}

	// 文字セット
	readFontCharset(fontCharset8, file, _T("CAPTION_CHARSET_8"));
	readFontCharset(fontCharset8, file, _T("ICON_CHARSET_8"));
	readFontCharset(fontCharset8, file, _T("SMALLCAPTION_CHARSET_8"));
	readFontCharset(fontCharset8, file, _T("STATUS_CHARSET_8"));
	readFontCharset(fontCharset8, file, _T("MESSAGE_CHARSET_8"));
	readFontCharset(fontCharset8, file, _T("MENU_CHARSET_8"));

	return 1;
}

/**
 * Windows 10のフォントプリセットリソースの読み込みを行う
 *
 * @param file リソースファイル名
 * @return 0:設定失敗 1:設定成功
 */
int readFontResource10(TCHAR *file)
{
	int result;

	// フォント名
	result = readFontFace(fontFaces10, file, _T("CAPTION_FACE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces10, file, _T("ICON_FACE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces10, file, _T("SMALLCAPTION_FACE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces10, file, _T("STATUS_FACE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces10, file, _T("MESSAGE_FACE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces10, file, _T("MENU_FACE_10"));
	if (result == 0) {
		return 0;
	}

	// 文字サイズ
	result = readFontSize(fontSizes10, file, _T("CAPTION_SIZE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes10, file, _T("ICON_SIZE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes10, file, _T("SMALLCAPTION_SIZE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes10, file, _T("STATUS_SIZE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes10, file, _T("MESSAGE_SIZE_10"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes10, file, _T("MENU_SIZE_10"));
	if (result == 0) {
		return 0;
	}

	// 文字セット
	readFontCharset(fontCharset10, file, _T("CAPTION_CHARSET_10"));
	readFontCharset(fontCharset10, file, _T("ICON_CHARSET_10"));
	readFontCharset(fontCharset10, file, _T("SMALLCAPTION_CHARSET_10"));
	readFontCharset(fontCharset10, file, _T("STATUS_CHARSET_10"));
	readFontCharset(fontCharset10, file, _T("MESSAGE_CHARSET_10"));
	readFontCharset(fontCharset10, file, _T("MENU_CHARSET_10"));

	return 1;
}

/**
 * Windows 11のフォントプリセットリソースの読み込みを行う
 *
 * @param file リソースファイル名
 * @return 0:設定失敗 1:設定成功
 */
int readFontResource11(TCHAR* file)
{
	int result;

	// フォント名
	result = readFontFace(fontFaces11, file, _T("CAPTION_FACE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces11, file, _T("ICON_FACE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces11, file, _T("SMALLCAPTION_FACE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces11, file, _T("STATUS_FACE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces11, file, _T("MESSAGE_FACE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontFace(fontFaces11, file, _T("MENU_FACE_11"));
	if (result == 0) {
		return 0;
	}

	// 文字サイズ
	result = readFontSize(fontSizes11, file, _T("CAPTION_SIZE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes11, file, _T("ICON_SIZE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes11, file, _T("SMALLCAPTION_SIZE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes11, file, _T("STATUS_SIZE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes11, file, _T("MESSAGE_SIZE_11"));
	if (result == 0) {
		return 0;
	}
	result = readFontSize(fontSizes11, file, _T("MENU_SIZE_11"));
	if (result == 0) {
		return 0;
	}

	// 文字セット
	readFontCharset(fontCharset11, file, _T("CAPTION_CHARSET_11"));
	readFontCharset(fontCharset11, file, _T("ICON_CHARSET_11"));
	readFontCharset(fontCharset11, file, _T("SMALLCAPTION_CHARSET_11"));
	readFontCharset(fontCharset11, file, _T("STATUS_CHARSET_11"));
	readFontCharset(fontCharset11, file, _T("MESSAGE_CHARSET_11"));
	readFontCharset(fontCharset11, file, _T("MENU_CHARSET_11"));

	return 1;
}

/**
 * 日本語版Windows 8のフォントプリセットリソースの設定を行う
 *
 * @return 1:設定成功
 */
int setFontResourceJa8(void)
{
	fontFaces8.push_back(_T("Meiryo UI"));
	fontFaces8.push_back(_T("Meiryo UI"));
	fontFaces8.push_back(_T("Meiryo UI"));
	fontFaces8.push_back(_T("Meiryo UI"));
	fontFaces8.push_back(_T("Meiryo UI"));
	fontFaces8.push_back(_T("Meiryo UI"));

	fontSizes8.push_back(11);
	fontSizes8.push_back(9);
	fontSizes8.push_back(11);
	fontSizes8.push_back(9);
	fontSizes8.push_back(9);
	fontSizes8.push_back(9);

	fontCharset8.push_back(1);
	fontCharset8.push_back(1);
	fontCharset8.push_back(1);
	fontCharset8.push_back(1);
	fontCharset8.push_back(1);
	fontCharset8.push_back(1);

	return 1;
}

/**
 * 日本語版Windows 8のフォントプリセットリソースの設定を行う
 *
 * @return 1:設定成功
 */
int setFontResourceJa10(void)
{
	fontFaces10.push_back(_T("Yu Gothic UI"));
	fontFaces10.push_back(_T("Yu Gothic UI"));
	fontFaces10.push_back(_T("Yu Gothic UI"));
	fontFaces10.push_back(_T("Yu Gothic UI"));
	fontFaces10.push_back(_T("Yu Gothic UI"));
	fontFaces10.push_back(_T("Yu Gothic UI"));

	fontSizes10.push_back(9);
	fontSizes10.push_back(9);
	fontSizes10.push_back(9);
	fontSizes10.push_back(9);
	fontSizes10.push_back(9);
	fontSizes10.push_back(9);

	fontCharset10.push_back(1);
	fontCharset10.push_back(1);
	fontCharset10.push_back(1);
	fontCharset10.push_back(1);
	fontCharset10.push_back(1);
	fontCharset10.push_back(1);

	return 1;
}

/**
 * ウインドウを親ウインドウの中央に配置する。
 *
 * @param parentRect 中心に入れる対象の領域
 * @param parentHWnd 親ウインドウハンドル
 * @param myHWnd 中央寄せする要素のウインドウハンドル
 */
void adjustCenter(RECT parentRect, HWND parentHWnd, HWND myHWnd)
{
	int parentWidth, parentHeight;
	int myWidth, myHeight;
	int newTop, newLeft;
	RECT myRect;

	GetWindowRect(myHWnd, &myRect);

	parentWidth = parentRect.right - parentRect.left + 1;
	parentHeight = parentRect.bottom - parentRect.top + 1;

	myWidth = myRect.right - myRect.left + 1;
	myHeight = myRect.bottom - myRect.top + 1;

	double scale = (double)getSystemDPI() / 96;
	int targetWidth = REQUIRED_CLIENT_WIDTH * scale;
	int targetHeight = REQUIRED_CLIENT_HEIGHT * scale;

	if (myWidth >= parentWidth) {
		newLeft = parentRect.left + 1;
	} else {
		newLeft = parentRect.left + MulDiv((parentWidth - myWidth), 45, 100);
	}
	if (myHeight >= parentHeight) {
		newTop = parentRect.top + 1;
	} else {
		newTop = parentRect.top + MulDiv((parentHeight - myHeight), 45, 100);
	}

	SetWindowPos(myHWnd, parentHWnd, newLeft, newTop, myWidth, myHeight, SWP_SHOWWINDOW);

}

/**
 * 韓国語のフォント名を置き換える(韓国語専用)
 * 
 * @param dispBuf 韓国語のフォント名
 */
void getKoreanFontName(TCHAR *dispBuf)
{
	if (!_tcscmp(dispBuf, _T("Gulim"))) {
		_tcscpy(dispBuf, _T("굴림"));
	}

	if (!_tcscmp(dispBuf, _T("GulimChe"))) {
		_tcscpy(dispBuf, _T("굴림체"));
	}

	if (!_tcscmp(dispBuf, _T("Gungsuh"))) {
		_tcscpy(dispBuf, _T("궁서"));
	}

	if (!_tcscmp(dispBuf, _T("GungsuhChe"))) {
		_tcscpy(dispBuf, _T("궁서체"));
	}

	if (!_tcscmp(dispBuf, _T("Dotum"))) {
		_tcscpy(dispBuf, _T("돋움"));
	}

	if (!_tcscmp(dispBuf, _T("DotumChe"))) {
		_tcscpy(dispBuf, _T("돋움체"));
	}

	if (!_tcscmp(dispBuf, _T("Malgun Gothic"))) {
		_tcscpy(dispBuf, _T("맑은 고딕"));
	}

	if (!_tcscmp(dispBuf, _T("Batang"))) {
		_tcscpy(dispBuf, _T("바탕"));
	}

	if (!_tcscmp(dispBuf, _T("BatangChe"))) {
		_tcscpy(dispBuf, _T("바탕체"));
	}

}

/**
 * 文字列を置き換える
 *
 * @param buf 置き換えられた文字列
 * @param source 置き換える対象の文字列
 * @param oldWord 置換される文字列
 * @param newWord 置換する文字列
 * @param bufLen 置き換え先文字列のバッファサイズ
 *
 */
void strreplace(TCHAR* buf, const TCHAR* source, const TCHAR* oldWord, const TCHAR* newWord, int bufLen)
{
	int p = 0;
	int newP = 0;
	int oldLen;
	int newLen;
	int copyLen;

	oldLen = _tcslen(oldWord);
	newLen = _tcslen(newWord);

	while (*(source + p) != _T('\0')) {
		if (_tcsncmp(source + p, oldWord, oldLen) == 0) {
			if ((newP + newLen) < bufLen) {
				_tcscpy(buf + newP, newWord);
				newP += newLen;
			} else {
				copyLen = bufLen - newP;
				_tcsncpy(buf + newP, newWord, copyLen - 1);
				newP += (copyLen - 1);
			}
			p += oldLen;
		} else {
			if (newP >= bufLen) {
				return;
			} else if (newP == (bufLen - 1)) {
				*(buf + newP) = _T('\0');
				return;
			} else {
				*(buf + newP) = *(source + p);
				newP++;
				p++;
			}
		}
	}
	if (newP < bufLen) {
		*(buf + newP) = _T('\0');
	}
}

/**
 * Windows 11かどうか判別する。
 *
 * @param buildNumber ビルド番号格納先
 * @return TRUE:Windows 11 or later FALSE:Windows 10
 */
BOOL isWin11OrLater(DWORD &buildNumber)
{
	OSVERSIONINFOEX versionInfo;
	DWORDLONG dwlConditionMask = 0;
	int op = VER_GREATER_EQUAL;

	// Initialize the OSVERSIONINFOEX structure.

	ZeroMemory(&versionInfo, sizeof(OSVERSIONINFOEX));
	versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	versionInfo.dwMajorVersion = 10;
	versionInfo.dwMinorVersion = 0;
	versionInfo.wServicePackMajor = 0;
	versionInfo.wServicePackMinor = 0;
	versionInfo.dwBuildNumber = 22000;

#if 0
	// Initialize the condition mask.

	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMAJOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_SERVICEPACKMINOR, op);
	VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, op);

	// Perform the test.

	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION |
		VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR | VER_BUILDNUMBER,
		dwlConditionMask);
#endif

	GetVersionEx((OSVERSIONINFO *)&versionInfo);

	buildNumber = versionInfo.dwBuildNumber;
	if (versionInfo.dwBuildNumber >= 22000) {	// 22000 is first version(21H2)
		return TRUE;
	} else{
		return FALSE;
	}
}

/**
 * アプリケーションが実用するWindowsのバージョンを返す(Windows 11対応)<br>
 * Windows 11はメジャーバージョン11扱いとする。その後のWindowsがメジャーバージョン11返したらその時は10.xx扱いにする。<br>
 * Windows 7(6.1)/8(6.2)/8.1(6.3)は既存コードとの兼ね合いからそのままとする。
 * 
 * @return 上位16ビット：メジャーバージョン 下位16ビット：マイナーバージョン
 */
DWORD GetVersionForApp(DWORD& majorVersion, DWORD& minorVersion, DWORD &buildNumber)
{
	DWORD dwVersion = GetVersion();

	majorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	minorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	if (majorVersion == 10 && isWin11OrLater(buildNumber)) {
		// Windows 11が普通にやると10返すのでここで１１と判別
		majorVersion = 11;
	}

	return (majorVersion << 16) | minorVersion;
}

/**
 * charをunsignedとみなしてintに変換する
 * 
 * @param c 変換するchar
 * @return cをunsignedとみなして変換した値
 */
inline int charToInt(char c)
{
	if (c < 0) {
		return 256 + c;
	}
	else {
		return c;
	}

}

/**
 * UTF-8文字列をUTF-16文字列に変換する
 *
 * @param src 変換元UTF-8文字列
 * @param dst 変換先領域(NULL可能)
 * @return 必要となるwchar_tの個数
 */
size_t utf8toUtf16(tstring &dst, const char* src)
{
	wchar_t wc;
	int shift_count;
	int i;

	assert(src != NULL);

	while (*src) {
		// Extruct to Unicode
		int c = charToInt(*src);

		wc = 0;
		if (c < 0x80) {
			// 1byte(7bit)
			wc = c;
			shift_count = 0;
		}
		else if (c >= 0xfc) {
			// 6bytes(27-32bit:no use)
			wc = c & 0x01;
			shift_count = 5;
		}
		else if (c >= 0xf8) {
			// 5bytes(22-26bit:no use)
			wc = c & 0x03;
			shift_count = 4;
		}
		else if (c >= 0xf0) {
			// 4bytes(17-21bit)
			wc = c & 0x07;
			shift_count = 3;
		}
		else if (c >= 0xe0) {
			// 3bytes(12-16bit)
			wc = c & 0x0f;
			shift_count = 2;
		}
		else if (c >= 0xc0) {
			// 2bytes(8-11bit)
			wc = c & 0x1f;
			shift_count = 1;
		}
		else {
			break;
		}

		src++;
		for (i = 0; i < shift_count; i++) {
			if (*src == '\0') {
				wc = L'\0';
				break;
			}
			c = charToInt(*src);

			wc = wc << 6;
			wc = wc | (c & 0x3f);
			src++;
		}

		if (c >= 0x10000) {
			// Windows uses UTF-16 LE
			int c1 = (c & 0x3ff) | 0xdc00;
			int c2_1 = (c >> 10) | 0x3f;
			int c2_2 = ((c >> 16) & 0x1f) - 1;
			int c2 = 0xd800 | c2_2 | c2_1;

			dst += c1;
			dst += c2;
		}
		else {
			dst += wc;
		}
	}

	return dst.length();
}

/**
 * システムのDPIを取得する。
 * 
 * @return システムのDPI
 */
int getSystemDPI(void)
{
	HDC hDc = GetDC(GetDesktopWindow());
	int dpi = GetDeviceCaps(hDc, LOGPIXELSY);

	return dpi;
}