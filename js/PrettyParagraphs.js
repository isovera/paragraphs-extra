/**
 * @file
 * Defines plugins for the Pretty Paragraphs UI enhancements.
 */

(function ($) {

  Drupal.ParagraphsExtra = Drupal.ParagraphsExtra || {};
  Drupal.ParagraphsExtra.Api = Drupal.ParagraphsExtra.Api || {};

  /**
   * Extension to the ParagraphsExtra.Api to attach the PrettyParagraphs behavior.
   */
  Drupal.ParagraphsExtra.Api.PrettyParagraphs = {

    /**
     * Initializes the PrettyParagraphs behaviors.
     */
    init: function() {
      Drupal.ParagraphsExtra.Api.Paragraphs.listener.registerPlugin(
        new Drupal.ParagraphsExtra.Api.PrettyParagraphs.ClickToEditPlugin()
      );
      Drupal.ParagraphsExtra.Api.FieldInstances.listener.registerPlugin(
        new Drupal.ParagraphsExtra.Api.PrettyParagraphs.ViewModePlugin()
      );
      Drupal.ParagraphsExtra.Api.FieldInstances.listener.registerPlugin(
        new Drupal.ParagraphsExtra.Api.PrettyParagraphs.AddModalPlugin()
      );
      Drupal.ParagraphsExtra.Api.Paragraphs.listener.registerPlugin(
        new Drupal.ParagraphsExtra.Api.PrettyParagraphs.InsertPlugin()
      );
    }
  };

  /**
   * A ParagraphsExtra.Api plugin for providing 'click-to-edit' functionality.
   */
  Drupal.ParagraphsExtra.Api.PrettyParagraphs.ClickToEditPlugin = function() {

    this.name = 'PrettyParagraphs.ClickToEdit';

    var plugin = this;
    var $ = jQuery;
    var button_class = Drupal.ParagraphsExtra.className('pretty-paragraphs', 'button');
    var toolbar_class = Drupal.ParagraphsExtra.className('pretty-paragraphs', 'edit-toolbar');

    this.settings = {
      defaultShowEffect: 'show',
      defaultHideEffect: 'hide'
    };

    /**
     * Called when a paragraph item is first created.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The newly created paragraph.
     */
    this.onCreate = function(paragraph) {
      // Hide the paragraphs ui remove button
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'delete-button', paragraph.$container).hide();

      // Add event handlers for the paragraph row.
      paragraph.$container.click(function(e) { plugin._onRowClick(paragraph, e); })
        .hover(function(e) { 
          e.preventDefault();
          plugin._onRowHover(paragraph, e); 
        })
        .mouseleave(function(e) { plugin._onMouseLeave(paragraph, e); });

      // Add event handlers for delete button.
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'button--delete', paragraph.$container)
        .click(function (e) {
          e.preventDefault();
          Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'delete-button', paragraph.$container).mousedown();
        })
        .hover(function(e) { plugin._onMouseLeave(paragraph, e); })
        .mouseleave(function(e) { plugin._onRowHover(paragraph, e); });

      // Add event handlers for swap buttons.
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'button--switch', paragraph.$container)
        .click(function (e) {
          e.preventDefault();
        });

      // Support the insert plugin.
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'button--add', paragraph.$container)
        .hover(function(e) { plugin._onMouseLeave(paragraph, e); })
        .mouseleave(function(e) { plugin._onRowHover(paragraph, e); });
    };

    /**
     * Called when a paragraph item is delivered via ajax.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The ajax-updated paragraph.
     */
    this.onAjax = function(paragraph, event_name, context) {
      // Set new content to edit mode by default.
      paragraph.$container.find('.ajax-new-content').each(function () {
        $(this).removeClass('ajax-new-content');
        plugin.expand(paragraph);
      });
    }

    /**
     * Called when a paragraph item is either created or delivered via ajax.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The updated paragraph.
     */
    this.onUpdate = function(paragraph, context) {
      // Switch between edit and preview mode depending on the value of the field.
      this.setMode(paragraph, Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-mode', paragraph.$container).val());

      // Show or hide the toolbar depending on whether we are in delete
      // confirmation mode.
      if (!paragraph.$container.find('.paragraphs-edit-confirm-delete').length) {
        Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-toolbar', paragraph.$container).removeClass('element-invisible');
        paragraph.$container.find('.form-actions .paragraphs-edit-delete').hide();
      }
    }

    /**
     * Expands the edit form for the paragraph item.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The paragraph item to expand the edit form for.
     * @param string showEffect
     *  The 'fade in' effect to use.
     * @param string hideEffect
     *  The 'fade out' effect to use.
     */
    this.expand = function(paragraph, showEffect, hideEffect) {
      var effects = loadEffects(this.settings, showEffect, hideEffect);

      // Swap out the edit and collapse buttons.
      paragraph.$container.find('.' + button_class + '--switch')
        .removeClass(button_class + '--edit')
        .addClass(button_class + '--collapse')
        .attr('title', Drupal.t('Collapse Section'))
        .parents('.' + toolbar_class)
        .addClass(toolbar_class + '--expanded');
      paragraph.$container.find('.' + button_class + '--switch .' + Drupal.ParagraphsExtra.className('mobile-only'))
        .html(Drupal.t('Collapse'));

      // Swap the edit, preview elements.
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-field', paragraph.$container).each(function() {
        $(this)[effects.showEffect]();
      });
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-preview', paragraph.$container).each(function() {
        $(this)[effects.hideEffect]();
      });

      // Set the edit mode to 'edit'.
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-mode', paragraph.$container).val('edit');
    }

    /**
     * Collapses the edit form and shows a preview for the paragraph row.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The paragraph item to collpase the edit form for.
     * @param string showEffect
     *  The 'fade in' effect to use.
     * @param string hideEffect
     *  The 'fade out' effect to use.
     */
    this.collapse = function(paragraph, showEffect, hideEffect) {
      var effects = loadEffects(this.settings, showEffect, hideEffect);

      // Swap out the edit and collapse buttons.
      paragraph.$container.find('.' + button_class + '--switch')
        .removeClass(button_class + '--collapse')
        .addClass(button_class + '--edit')
        .attr('title', Drupal.t('Edit Section'))
        .parents('.' + toolbar_class)
        .removeClass(toolbar_class + '--expanded');
      paragraph.$container.find('.' + button_class + '--switch .' + Drupal.ParagraphsExtra.className('mobile-only'))
        .html(Drupal.t('Edit'));

      // Swap the edit, preview elements.
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-field', paragraph.$container).each(function() {
        $(this)[effects.hideEffect]();
      });
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-preview', paragraph.$container).each(function() {
        $(this)[effects.showEffect]();
      });

      // Set the edit mode to 'preview'.
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-mode', paragraph.$container).val('preview');
    }

    /**
     * Sets the paragraphs edit mode to 'preview' and sends an ajax request to
     * refresh the preview with the current edit field state.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The paragraph object to refresh the viewmode for.
     */
    this.refreshPreview = function(paragraph) {
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-mode', paragraph.$container).val('preview');
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'preview-refresh', paragraph.$container).mousedown();
    }

    /**
     * Toggles a paragraph between edit and preview mode.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The paragraph to toggle the edit mode for.
     */
    this.toggle = function(paragraph) {
      // If the preview content is is currently visible we switch to edit mode,
      // otherwise we trigger a preview refresh from the form api.
      if (Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'edit-preview', paragraph.$container).is(':visible')) {
        this.expand(paragraph, 'fadeIn');
      }
      else {
        this.refreshPreview(paragraph);
      }
    }

    /**
     * Explicitly set the edit mode for a paragraph.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The paragraph to set the edit mode for.
     * @param string mode
     *  Should be either 'edit' or 'preview'.
     * @param string showEffect
     *  A jquery effect to use for the 'fade in' transition.
     * @param string hideEffect
     *  A jquery effect to use for the 'fade out' transition.
     */
    this.setMode = function(paragraph, mode, showEffect, hideEffect) {
      if (mode == 'edit') {
        this.expand(paragraph, showEffect, hideEffect);
      }
      else {
        this.collapse(paragraph, showEffect, hideEffect);
      }
    }
    
    /**
     * An internal event handler for handling clicks anywhere on the paragraphs
     * row.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The clicked paragraph.
     * @param object e
     *  The click event.
     */
    this._onRowClick = function(paragraph, e) {
      // We only want to perform a transition in cases where the paragram item is
      // clicked but the click independent of other actions such as form editing,
      // CTA clicks, etc.
      if (this._isRowBodyClick(paragraph, e) || this._isRowLabelClick(paragraph, e) || this._isSwitchButton(paragraph, e)) {
        this.toggle(paragraph);
      }
    }

    /**
     * Checks if a click event occured outside of an embedded Drupal form.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The clicked paragraph.
     * @param object e
     *  The click event.
     *
     * @return
     *  true if the click event occurred outside of an embedded form, false
     *  otherwise.
     */
    this._isRowBodyClick = function(paragraph, e) {
      var clicked_form_item = $(e.target).closest('.form-item').get(0)
      var paragraph_form_item = paragraph.$.closest('.form-item').get(0);
      var rtn = false;

      if (clicked_form_item && paragraph_form_item) {
        if (clicked_form_item === paragraph_form_item) {
          rtn = true;
        }
      }

      return rtn;
    }

    /**
     * Checks if the delete button was clicked.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The clicked paragraph.
     * @param object e
     *  The click event.
     *
     * @return
     *  true if the click occurred on the delete button, false otherwise.
     */
    this._isSwitchButton = function(paragraph, e) {
      var $closest_button = Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'button', $(e.target), 'parents');
      return $closest_button.hasClass(Drupal.ParagraphsExtra.className('pretty-paragraphs', 'button--switch'));
    }

    /**
     * Checks if a click occurred on a field label.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The clicked paragraph.
     * @param object e
     *  The click event.
     *
     * @return
     *  true if the click occurred on a label, false otherwise.
     */
    this._isRowLabelClick = function(paragraph, e) {
      var $elmt = $(e.target);
      var rtn = false;

      if ($elmt.attr('tagName').toLowerCase() == 'label') {
        if ($elmt.parent().hasClass('form-item')) {
          rtn = true;
        }
      }

      return rtn;
    }

    /**
     * Called when the user begins hovering over a paragraph row.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The paragraph where the user is hovering.
     */
    this._onRowHover = function(paragraph, e) {

      // Lets us style paragraph items that are being hovered over.
      paragraph.$container.addClass(Drupal.ParagraphsExtra.className('pretty-paragraphs', 'hover'));
    }

    /**
     * Called when a user stops hovering over a paragraph row.
     *
     * @param ParagraphsExtra.Api.Paragraphs.Paragraph paragraph
     *  The paragraph where the user was hovering.
     */
    this._onMouseLeave = function(paragraph, e) {
   
      // Remove hover styling.
      paragraph.$container.removeClass(Drupal.ParagraphsExtra.className('pretty-paragraphs', 'hover'));
    }

    /**
     * Internal function to resolve jQuery effects.
     *
     * @param object settings
     *  The settings object to load defaults from.
     * @param string showEffect
     *  An override for the 'fade in' effect.
     * @param string hideEffect
     *  An override for the 'fade out' effect.
     *
     * @return
     *  A settings object with the resolved effects.
     */
    function loadEffects(settings, showEffect, hideEffect) {
      var rtn = {};
      if (typeof showEffect == 'undefined') {
        rtn.showEffect = settings.defaultShowEffect;
      }
      else {
        rtn.showEffect = showEffect;
      }

      if (typeof hideEffect == 'undefined') {
        rtn.hideEffect = settings.defaultHideEffect;
      }
      else {
        rtn.hideEffect = hideEffect;
      }
      return rtn;
    }
  }

  Drupal.ParagraphsExtra.Api.PrettyParagraphs.ViewModePlugin = function(paragraph) {

    this.name = 'PrettyParagraphs.ViewMode';

    var $ = jQuery;
    var plugin = this;
    var button_class = Drupal.ParagraphsExtra.className('pretty-paragraphs', 'button');

    this.onCreate = function(field_instance) {
      var $toolbar = Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'viewmode-toolbar', field_instance.$);
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'button', $toolbar).click(function(e) {
        e.preventDefault();
        plugin.setViewMode(field_instance, Drupal.ParagraphsExtra.readData($(this), 'pretty-paragraphs', 'viewmode'));
      });
    }

    this.onUpdate = function(field_instance) {
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'viewmode-toolbar', field_instance.$).removeClass('element-invisible');
      setActiveButton(field_instance, Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'viewmode', field_instance.$).val());
    }

    this.setViewMode = function(field_instance, viewmode) {
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'viewmode', field_instance.$).val(viewmode);
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'viewmode-refresh', field_instance.$).mousedown();
      setActiveButton(field_instance, viewmode);
    }

    this.toggle = function(field_instance) {
      var $toolbar = Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'viewmode-toolbar', field_instance.$);
      var viewmode = Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'viewmode', field_instance.$).val();
      var before = [];
      var after = [];
      var list = before;

      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'button', $toolbar).each(function() {
        var current = Drupal.ParagraphsExtra.readData($(this), 'pretty-paragraphs', 'viewmode');
        if (current === viewmode) {
          list = after;
        }
        else {
          list.push(current);
        }
      });

      list = before.concat(after);

      this.setViewMode(field_instance, list.pop());
    }

    function setActiveButton(field_instance, viewmode) {
      var $toolbar = Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'viewmode-toolbar', field_instance.$);
      var view_mode_attr = Drupal.ParagraphsExtra.dataName('pretty-paragraphs', 'viewmode');

      $toolbar.find('.' + button_class + '--active').removeClass(button_class + '--active');
      $toolbar.find('.' + button_class + '[' + view_mode_attr + '="' + viewmode + '"]').addClass(button_class + '--active');

      if (!$toolbar.find('.' + button_class + '--active')) {
        $toolbar.find('.' + button_class + ':first').addClass('.' + button_class + '--active');
      }

      viewmode = $toolbar.find('.' + button_class + '--active').attr(view_mode_attr);
    }

  }

  Drupal.ParagraphsExtra.Api.PrettyParagraphs.AddModalPlugin = function() {

    this.name = 'PrettyParagraphs.AddModal';

    var plugin = this;
    var active_instance = null;

    this.onCreate = function(field_instance) {
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'button--add', field_instance.$container).click(function(e) {
        e.preventDefault();
        e.stopPropagation();
        active_instance = field_instance;
        Drupal.ParagraphsExtra.Api.BundleSelectModal.setActivePlugin(plugin);
        Drupal.ParagraphsExtra.Api.BundleSelectModal.show();
      });
    }

    this.onBundleSelect = function(modal, event_name, bundle) {
      var paragraphs = active_instance.getParagraphs().values;
      var command; 

      if (paragraphs.length) {
        command = paragraphs[paragraphs.length - 1].createCommand();
      }
      else {
        command = new Drupal.ParagraphsExtra.Api.ParagraphCommand(active_instance, 0);
      }

      command.insertAfter(bundle).execute();
    }
  }

  Drupal.ParagraphsExtra.Api.PrettyParagraphs.InsertPlugin = function() {

    this.name = 'PrettyParagraphs.Insert';

    var plugin = this;
    var active_paragraph = null;

    this.onCreate = function(paragraph) {
      Drupal.ParagraphsExtra.lookup('pretty-paragraphs', 'button--insert', paragraph.$container).click(function(e) {
        e.preventDefault();
        e.stopPropagation();
        active_paragraph = paragraph;
        Drupal.ParagraphsExtra.Api.BundleSelectModal.setActivePlugin(plugin);
        Drupal.ParagraphsExtra.Api.BundleSelectModal.show();
      });
    }

    this.onBundleSelect = function(modal, event_name, bundle) {
      active_paragraph.createCommand().insertBefore(bundle).execute();
    }
  }
}(jQuery));
