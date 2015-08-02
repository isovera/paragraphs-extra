/**
 * @file
 *
 * A CKEDITOR Plugin for integrating with the paragraphs module.
 */

function ParagraphsCkeditorData(editor) {
  var $ = jQuery;

  this.$editor = $(editor.element.$);
  this.$paragraph = Drupal.ParagraphsExtra.lookup('ajax-api', 'paragraphs-item', this.$editor, 'closest');
  this.$ckeditor_field = this.$editor.closest('.paragraphs-ckeditor-field');

  if (this.$paragraph) {
    this.paragraphs_instance = Drupal.ParagraphsExtra.Api.getFieldInstance(Drupal.ParagraphsExtra.readData(this.$paragraph, 'ajax-api', 'owner'));
    if (this.paragraphs_instance) {
      this.paragraph = this.paragraphs_instance.getParagraph(Drupal.ParagraphsExtra.readData(this.$paragraph, 'ajax-api', 'delta'));
    }
    else {
      this.paragraphs_instance = null;
    }
  }
  else {
    this.paragraph = null;
  }

  if (this.$paragraph && this.paragraph && this.paragraphs_instance) {
    this.valid = true;

    this.singular = this.paragraphs_instance.getInstanceSetting('title');
    if (!this.singular) {
      this.singular = 'paragraph';
    }
  }
  else {
    this.valid = false;
  }
}
