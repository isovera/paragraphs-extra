/**
 * @file
 *
 * A CKEDITOR Plugin for integrating with the paragraphs module.
 */

( function() {
  "use strict";

  // Register CKEDITOR plugin
  CKEDITOR.plugins.add( 'paragraphsSplitParagraph', {
    requires : [ "dialog" ],
    init : function (editor) {
      var data = new ParagraphsCkeditorData(editor);
      if (data.valid) {
        editor.addCommand( 'paragraphsSplitParagraph', new CKEDITOR.dialogCommand('paragraphsSplitParagraphConfirmation') );
        editor.ui.addButton( 'paragraphsSplitParagraph', {
          label: 'Split' + data.singular,
          command: 'paragraphsSplitParagraph',
          toolbar: 'insert',
          icon: this.path + '/images/split.png',
        } );
        CKEDITOR.dialog.add( 'paragraphsSplitParagraphConfirmation', this.path + 'dialogs/confirmSplit.js' );
      }
    },
  } );

} )();
