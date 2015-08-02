/**
 * @file
 *
 * A CKEDITOR dialog for selecting a paragraph bundle.
 */
"use strict";

CKEDITOR.dialog.add( 'paragraphsSplitParagraphConfirmation', function ( editor ) {

  // Populate local helper variables
  var $ = jQuery;
  var data = new ParagraphsCkeditorData(editor);

  // CKEDITOR dialog definition.
  return {
    title: 'Split ' + data.singular,
    minWidth: 300,
    minHeight: 100,
    contents: [
      {
        id: 'tab-basic',
        label: 'Basic Settings',
        elements: [
          {
            type: 'html',
            id: 'description',
            html: 'This tool will split the section you are working on into two sections.<br><br>Click Ok to continue.',
          },
        ]
      }
    ],
    onOk: function() {
      var partitioner = new CkeditorDomSplitter(data.$ckeditor_field);
      var partitioned = partitioner.partitionTree(editor.getSelection().getRanges()[0]);
      if (partitioned.hasOwnProperty('left') || partitioned.hasOwnProperty('right')) {
        var command = data.paragraph.createCommand();

        // Handle left partition.
        if (partitioned.hasOwnProperty('left')) {
          command.replace(data.paragraph.bundle.bundle_name, partitioned.left);
        }

        // Handle right partition.
        if (partitioned.hasOwnProperty('right')) {
          if (command.data.replace) {
            command.insertAfter(data.paragraph.bundle.bundle_name, partitioned.right);
          }
          else {
            command.replace(data.paragraph.bundle.bundle_name, partitioned.right);
          }
        }
        command.execute();
      }
    },
  };

});
