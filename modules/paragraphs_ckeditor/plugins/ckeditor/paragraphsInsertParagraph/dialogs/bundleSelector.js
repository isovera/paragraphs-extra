/**
 * @file
 *
 * A CKEDITOR dialog for selecting a paragraph bundle.
 */
"use strict";

CKEDITOR.dialog.add( 'paragraphsInsertParagraphBundleSelector', function ( editor ) {


  // CKEDITOR dialog definition.
  return {
    title: 'Add ' + data.singular,
    minWidth: 1,
    minHeight: 1,
    contents: [
      {
        id: 'tab-basic',
        label: 'Basic Settings',
        elements: [
        ]
      }
    ],
  };

});
