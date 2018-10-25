#include <string>
#include <iostream>
#include <vector>

#include <poppler/PDFDoc.h>
#include <poppler/goo/GooString.h>
#include <poppler/PDFDocFactory.h>
#include <poppler/Outline.h>
#include <poppler/GlobalParams.h>
#include <poppler/UnicodeMap.h>
#include <poppler/Link.h>
#include <poppler/Object.h>


using namespace std;
static string* unicode_to_char(const Unicode *unicode, int len) {
	static GlobalParams *globalParams = NULL;
	if (globalParams == NULL) {
		globalParams = new GlobalParams();
	}

	static UnicodeMap *uMap = NULL;
	if (uMap == NULL) {
		GooString *enc = new GooString("UTF-8");
		uMap = globalParams->getUnicodeMap(enc);
		uMap->incRefCnt();
		delete enc;
	}

	GooString gstr;
	char buf[8];
	int i, n;

	for (i = 0; i < len; ++i) {
		n = uMap->mapUnicode(unicode[i], buf, sizeof(buf));
		gstr.append(buf, n);
	}

	return new string(gstr.getCString());
}

PDFDoc *OpenDoc(char *filename)
{
	GooString fname(filename);
	auto doc = PDFDocFactory().createPDFDoc(fname);
	return doc;
}

const char *getPaddingStar(int num)
{
	static char *buf = NULL;
	static int bufLen = -1;

	if (num > bufLen) {
		if (bufLen > 0)
			delete[] buf;

		bufLen = num * 2 + 16;
		buf = new char[bufLen+1];
		for (int i = 0; i < bufLen; i++)
			buf[i] = '*';
		buf[bufLen] = '\0';
	}
	return buf + bufLen - num;
}

void printOutline(PDFDoc *doc, const GooList *items, int depth)
{
	if (items == NULL)
		return;

	for (int i = 0; i < items->getLength(); i++) {
		OutlineItem *item = (OutlineItem *) items->get(i);

		// Get title
		string *title = unicode_to_char(item->getTitle(), item->getTitleLength());

		int page_num = 0;

		// Get destination
		const LinkAction *action = item->getAction();
		if (action != nullptr) {
			if (action->getKind() != LinkActionKind::actionGoTo)
				continue;
			LinkGoTo *actionGoTo = (LinkGoTo *) action;
			if (!actionGoTo->isOk())
				continue;

			const LinkDest *dest = actionGoTo->getDest();
			if (dest == NULL) {
				const GooString *namedDest = actionGoTo->getNamedDest();
				dest = doc->findDest(namedDest);
			}

			if (dest == NULL || !dest->isOk())
				continue;

			if (dest->isPageRef()) {
				Ref page_ref = dest->getPageRef();
				page_num = doc->findPage(page_ref.num, page_ref.gen);
			} else {
				page_num = dest->getPageNum();
			}
		}

		// Print the title and page number
		std::cout << getPaddingStar(depth) << "!" << *title << " " << page_num << "\n";

		// Print kids
		if (!item->hasKids())
			continue;

		item->open();
		const GooList *kids = item->getKids();
		printOutline(doc, kids, depth+1);
	}
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "No pdf document given" << std::endl;
		exit(1);
	}
	GooString fname(argv[1]);
	PDFDoc *doc = PDFDocFactory().createPDFDoc(fname);
	if (!doc->isOk()) {
		delete doc;
		exit(1);
	}

	Outline *outline = doc->getOutline();
	const GooList *items = outline->getItems();
	printOutline(doc, items, 0);
	delete doc;

	return 0;
}
