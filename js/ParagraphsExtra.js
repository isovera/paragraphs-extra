/**
 * @file
 * Helper functions for the Paragraphs Extra module.
 */

(function ($) {
  Drupal.ParagraphsExtra = Drupal.ParagraphsExtra || {};

  Drupal.ParagraphsExtra.className = function(namespace, name) {
    var cls = 'paragraphs-extra-' + namespace;
    if (typeof name !== undefined) {
      cls += '-' + name;
    }
    return cls;
  };

  Drupal.ParagraphsExtra.dataName = function(namespace, name) {
    var data = 'data-paragraphs-extra-' + namespace;
    if (typeof name !== undefined) {
      data += '-' + name;
    }
    return data;
  };

  Drupal.ParagraphsExtra.lookup = function(namespace, name, $context, method) {
    var selector = '.' + this.className(namespace, name);
    var rtn = null;
    if (typeof $context !== 'undefined') {
      if (typeof method !== 'undefined') {
        rtn = $context[method](selector);
      }
      else {
        rtn = $context.find(selector);
      }
    }
    else {
      rtn = jQuery(selector);
    }
    return rtn;
  };

  Drupal.ParagraphsExtra.readData = function($element, namespace, name) {
    return $element.attr(this.dataName(namespace, name));
  };

  Drupal.ParagraphsExtra.writeData = function($element, namespace, name, value) {
    $element.attr(this.dataName(namespace, name, value));
  };
}(jQuery));
