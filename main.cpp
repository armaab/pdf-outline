#include <string>
#include <sstream>
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

static string unicode_to_char(const Unicode *unicode, int len) {
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

	stringstream ss;
	char buf[8];
	int i, n;

	for (i = 0; i < len; ++i) {
		n = uMap->mapUnicode(unicode[i], buf, sizeof(buf));
		ss.write(buf, n);
	}

	return ss.str();
}

PDFDoc *OpenDoc(char *filename)
{
	GooString fname(filename);
	auto doc = PDFDocFactory().createPDFDoc(fname);
	return doc;
}

void printOutline(PDFDoc *doc, const vector<OutlineItem*> *items, int depth)
{
	if (items == nullptr) return;

	for (OutlineItem *item : *items) {
		// Get title
		auto title = unicode_to_char(item->getTitle(), item->getTitleLength());

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
				page_num = doc->findPage(page_ref);
			} else {
				page_num = dest->getPageNum();
			}
		}

		// Print the title and page number
		static constexpr char stars[] = "****************";
		static constexpr int len_stars = sizeof(stars) / sizeof(*stars) - 1;
		int d = depth;
		while (d > len_stars) {
			cout << stars;
			d -= len_stars;
		}
		cout << (stars + len_stars - d) << "!";
		cout << title << " " << page_num << "\n";

		// Print kids
		if (!item->hasKids())
			continue;

		item->open();
		const vector<OutlineItem*> *kids = item->getKids();
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
	const vector<OutlineItem*> *items = outline->getItems();
	printOutline(doc, items, 0);
	delete doc;

	return 0;
}
