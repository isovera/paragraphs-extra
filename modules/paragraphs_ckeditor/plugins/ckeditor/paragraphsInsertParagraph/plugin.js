/**
 * @file
 *
 * A CKEDITOR Plugin for integrating with the paragraphs module.
 */

( function($) {
  "use strict";

  var showBundleSelectModal = function (editor) {

    // Populate local helper variables
    var data = new ParagraphsCkeditorData(editor);

    var SelectedBundleHandler = {

      onBundleSelect: function(modal, event_name, bundle) {
        var partitioner = new CkeditorDomSplitter(data.$ckeditor_field);
        var partitioned = partitioner.partitionTree(editor.getSelection().getRanges()[0]);
        if (partitioned.hasOwnProperty('left') || partitioned.hasOwnProperty('right')) {
          var command = data.paragraph.createCommand();

          // Handle left partition.
          if (partitioned.hasOwnProperty('left')) {
            command.replace(data.paragraph.bundle.bundle_name, partitioned.left);
          }

          // Handle new paragraph.
          if (command.data.replace) {
            command.insertAfter(bundle);
          }
          else {
            command.insertBefore(bundle);
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
      }
    }

    Drupal.ParagraphsExtra.Api.BundleSelectModal.setActivePlugin(SelectedBundleHandler);
    Drupal.ParagraphsExtra.Api.BundleSelectModal.show();
  }


  // Register CKEDITOR plugin
  CKEDITOR.plugins.add( 'paragraphsInsertParagraph', {
    requires : [ "dialog" ],
    init : function (editor) {
      var data = new ParagraphsCkeditorData(editor);
      if (data.valid) {
        editor.addCommand( 'paragraphsInsertParagraph', { exec: showBundleSelectModal });
        editor.ui.addButton( 'paragraphsInsertParagraph', {
          label: 'Insert ' + data.singular,
          command: 'paragraphsInsertParagraph',
          toolbar: 'insert',
          icon: this.path + '/images/component.png',
        } );
        CKEDITOR.dialog.add( 'paragraphsInsertParagraphBundleSelector', this.path + 'dialogs/bundleSelector.js' );
      }
    },
  } );

} )(jQuery);
