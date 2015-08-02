(function ($) {

  Drupal.ParagraphsExtra = Drupal.ParagraphsExtra || {};
  Drupal.ParagraphsExtra.Api = Drupal.ParagraphsExtra.Api || {};

  Drupal.ParagraphsExtra.Api.BundleSelectModal = {

    init: function(plugin) {
      plugin.listener = new Drupal.ParagraphsExtra.Api.EventListener({
        bundle_selected: 'onBundleSelect',
      });
    },

    setActivePlugin: function(plugin) {
      this.listener.plugins.values[0] = plugin;
    },

    triggerActivePlugin: function(bundle) {
      this.listener.trigger(this, 'bundle_selected', bundle);
    },

    show: function() {
      Drupal.ParagraphsExtra.lookup('bundle-info', 'modal-trigger').click();
    }

  };

  Drupal.behaviors.ParagraphsExtraBundleSelector = {
    attach: function (context, settings) {
      $('#modal-content .paragraphs-extra-bundle-option', context).once().hover(function(e) {
        $(this).addClass('paragraphs-extra-bundle-option--hover');
      })
      .mouseleave(function(e) {
        $(this).removeClass('paragraphs-extra-bundle-option--hover');
      })
      .click(function(e) {
        Drupal.CTools.Modal.dismiss();
        Drupal.ParagraphsExtra.Api.BundleSelectModal.triggerActivePlugin($(this).attr('data-paragraphs-extra-bundle'));
      });
    }
  }
}(jQuery));
