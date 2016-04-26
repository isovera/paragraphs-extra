(function ($) {
  Drupal.behaviors.ParagraphsExtraWidgets = {
    attach: function (context, settings) {
      $('.paragraphs-ckeditor-tabs__tab a', context).once().click(function(e) {
        e.preventDefault();
        var href = $(this).attr('href');
        $('.paragraphs-extra-widget-value').attr('value', href.substring(6));
        $('.paragraphs-extra-widget-switch').mousedown();
      });
    }
  }
}(jQuery));
