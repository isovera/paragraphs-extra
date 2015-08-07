/**
 * @file
 * Provides a javascript API for working with the paragraphs module.
 */

(function ($) {

  Drupal.ParagraphsExtra = Drupal.ParagraphsExtra || {};
  Drupal.ParagraphsExtra.Api = Drupal.ParagraphsExtra.Api || {};

  /**
   * Defines the general API for dealing with paragraphs objects.
   */
  Drupal.ParagraphsExtra.Api.init = function(context, settings) {
    this.settings = new Drupal.ParagraphsExtra.Api.List();

    if (typeof Drupal.settings.ParagraphsExtra.Api !== 'undefined') {
      for (var setting in Drupal.settings.ParagraphsExtra.Api) {
        this.settings.pushKeyVal(setting, Drupal.settings.ParagraphsExtra.Api[setting]);
      }
    }

    this.trigger('init', context, settings);
  };

  Drupal.ParagraphsExtra.Api.load = function(context, settings) {
    this.trigger('load', context, settings);
  };

  Drupal.ParagraphsExtra.Api.refresh = function(context, settings) {
    var full_load = false;

    Drupal.ParagraphsExtra.lookup('ajax-api', 'field-instance', $(context)).each(function () {
      if (!$(this).data('ParagraphsExtra.Api.FieldInstance')) {
        full_load = true;
      }
    });

    if (full_load) {
      this.trigger('load', context, settings);
    }

    this.trigger('refresh', context, settings);
  };

  Drupal.ParagraphsExtra.Api.trigger = function(callback, context, settings) {
    for (var api in this) {
      if (typeof this[api] == 'object') {
        if (callback in this[api]) {
          this[api][callback].call(this, this[api], context, settings);
        }
      }
    }
  };

  /**
   * A general list object for storing key,value pairs.
   */
  Drupal.ParagraphsExtra.Api.List = function() {
    this.values = [];

    this.clear = function() {
    }

    this.pushKeyVal = function(key, value) {
      this.values[key] = value;
    }

    this.push = function(value) {
      this.values.push(value);
    }

    this.find = function(key) {
      if (key in this.values) {
        return this.values[key];
      }
      else {
        return null;
      }
    }

    this.each = function(callback) {
      for (var key in this.values) {
        callback.call(this, key, this.values[key]);
      }
    }
  };

  /**
   * An object for handling plugin events.
   */
  Drupal.ParagraphsExtra.Api.EventListener = function(events) {
    this.plugins = new Drupal.ParagraphsExtra.Api.List();
    this.callbacks = new Drupal.ParagraphsExtra.Api.List();

    this.listen = function(event_name, callback_name) {
      this.callbacks.pushKeyVal(event_name, callback_name);
    }

    this.registerPlugin = function(plugin) {
      this.plugins.pushKeyVal(plugin.name, plugin);
    }

    this.getPlugin = function(plugin_name) {
      return this.plugins.find(plugin_name);
    }

    this.trigger = function(triggering_object, event_name) {
      var callback = this.callbacks.find(event_name);
      var args = arguments;
      if (callback) {
        this.plugins.each(function (event_name, plugin) {
          if (plugin[callback]) {
            plugin[callback].apply(plugin, args);
          }
        });
      }
    }

    for (var event_name in events) {
      this.listen(event_name, events[event_name]);
    }
  }

  /**
   * Provides functionality related to paragraphs bundles.
   */
  Drupal.ParagraphsExtra.Api.Bundles = {
    
    load: function() {
      this.bundles = new Drupal.ParagraphsExtra.Api.List();
      var raw = this.settings.find('bundles');
      for (var instance_id in raw) {
        var instance_bundles = new Drupal.ParagraphsExtra.Api.List();
        for (var bundle_name in raw[instance_id]) {
          var bundle = new Drupal.ParagraphsExtra.Api.Bundles.Bundle(raw[instance_id][bundle_name]);
          if (bundle.bundle_name) {
            instance_bundles.pushKeyVal(bundle.bundle_name, bundle);
          }
          this.bundles.pushKeyVal(instance_id, instance_bundles);
        }
      }

      this.getBundle = function(instance_id, bundle_name) {
        var instance_bundles = this.getBundles(instance_id);
        if (instance_bundles) {
          return instance_bundles.find(bundle_name);
        }
        else {
          return null;
        }
      }

      this.getBundles = function(instance_id) {
        return this.bundles.find(instance_id);
      }
    },

    /**
     * An object representing a Paragraphs bundle.
     */
    Bundle: function(settings) {
      this.settings = settings;
      this.bundle_name = settings.bundle;

      this.getSetting = function(name) {
        if (name in this.settings) {
          return this.settings[name];
        }
        else {
          return null;
        }
      }
    },

  };

  /**
   * An object representing a paragraph action command to be executed with the
   * form API.
   */
  Drupal.ParagraphsExtra.Api.ParagraphCommand = function(field_instance, delta) {

    this.data = {
      delta: delta,
      insertBefore: [],
      insertAfter: [],
      replace: null,
      remove: false,
    };

    /**
     * Insert a paragraph before the delta.
     *
     * @param bundle
     * @param field
     * @param value
     */
    this.insertBefore = function(bundle, values) {
      this.data.insertBefore.push(createParagraph(bundle, values));
      return this;
    }

    /**
     * Insert a paragraph after the delta.
     *
     * @param bundle
     * @param field
     * @param value
     */
    this.insertAfter = function(bundle, values) {
      this.data.insertAfter.push(createParagraph(bundle, values));
      return this;
    }

    /**
     * Replace the paragraph at delta.
     *
     * @param bundle
     * @param field
     * @param value
     */
    this.replace = function(bundle, values) {
      this.data.replace = createParagraph(bundle, values);
      return this;
    }

    /**
     * Delete the delta. (WARNING: No confirmation is shown.)
     */
    this.remove = function() {
      this.data.remove = true;
    }

    /**
     * Send the ajax request to the server through the form API and let it work
     * its magic.
     */
    this.execute = function() {
      var instance_id = field_instance.instance_id;
      var $instance = field_instance.$;

      var data_selector = '.' + Drupal.ParagraphsExtra.className('ajax-api', 'command-data')
        + '[' + Drupal.ParagraphsExtra.dataName('ajax-api', 'owner') + '="' + instance_id + '"]';
      var command_selector = '.' + Drupal.ParagraphsExtra.className('ajax-api', 'command-trigger')
        + '[' + Drupal.ParagraphsExtra.dataName('ajax-api', 'owner') + '="' + instance_id + '"]';

      $instance.find(data_selector).val(JSON.stringify(this.data));
      $instance.find(command_selector).mousedown();

      return this;
    }

    /**
     * Create a paragraph object to send to the server.
     *
     * @param bundle
     * @param field
     * @param value
     */
    function createParagraph(bundle, values) {
      var paragraph = {
        bundle: bundle,
      };

      if (typeof values !== undefined) {
        paragraph.values = values;
      }

      return paragraph;
    }

  }

  Drupal.ParagraphsExtra.Api.Paragraphs = {

    refresh: function(instances, context) {

      Drupal.ParagraphsExtra.lookup('ajax-api', 'paragraphs-item', $(context)).each(function() {
        var paragraph = $(this).data('ParagraphsExtra.Api.Paragraph');
        if (!paragraph) {
          paragraph = new Drupal.ParagraphsExtra.Api.Paragraphs.Paragraph($(this), Drupal.ParagraphsExtra.readData($(this), 'ajax-api', 'delta'));
          paragraph.loadEvents.push(['create', context]);
        }
        else {
          paragraph.loadEvents.push(['ajax', context]);
        }
        paragraph.loadEvents.push(['update', context]);
      });
    },

    listener: new Drupal.ParagraphsExtra.Api.EventListener({
      create: 'onCreate',
      ajax: 'onAjax',
      update: 'onUpdate',
    }),

    Paragraph: function($paragraph, delta) {

      this.$ = $paragraph;
      this.delta = delta;
      this.loadEvents = [];

      this.$.data('ParagraphsExtra.Api.Paragraph', this);

      this.setInstance = function(instance) {
        this.instance = instance;
        this.bundle = Drupal.ParagraphsExtra.Api.getBundle(instance.instance_id, Drupal.ParagraphsExtra.readData($paragraph, 'ajax-api', 'bundle'));

        var $container = $paragraph;
        var selector = '[' + Drupal.ParagraphsExtra.dataName('ajax-api', 'instance-id') + '="' + instance.instance_id + '"]';
        $paragraph.parentsUntil(selector).each(function () {
          if ($(this).is('tr,li')) {
            $container = $(this);
          }
        });
        this.$container = $container;
        this.$container.data('ParagraphsExtra.Api.Paragraph', this);
      }

      this.createCommand = function() {
        return new Drupal.ParagraphsExtra.Api.ParagraphCommand(this.instance, this.delta);
      }
    }
  }

  Drupal.ParagraphsExtra.Api.Fields = {

    load: function() {

      var fields = new Drupal.ParagraphsExtra.Api.List();
      Drupal.ParagraphsExtra.lookup('ajax-api', 'field-instance').each(function() {
        var field_name = Drupal.ParagraphsExtra.readData($(this), 'ajax-api', 'field-name');
        var field = new Drupal.ParagraphsExtra.Api.Fields.Field(field_name);
        fields.pushKeyVal(field_name, field);
      });
      this.fields = fields;

      this.getField = function(field_name) {
        return this.fields.find(field_name);
      }

      this.getFields = function() {
        return fields;
      }
    },

    Field: function(field_name) {

      this.field_name = field_name;
      this.instances = new Drupal.ParagraphsExtra.Api.List();

      this.getInstance = function(instance_id) {
        return this.instances.find(instance_id);
      }

      this.getInstances = function() {
        return this.instances;
      }
    }

  };

  Drupal.ParagraphsExtra.Api.FieldInstances = {

    instances: new Drupal.ParagraphsExtra.Api.List(),

    refresh: function(instances, context) {

      var instances = new Drupal.ParagraphsExtra.Api.List();
      var api = this;
      Drupal.ParagraphsExtra.lookup('ajax-api', 'field-instance', $(context)).each(function() {
        var field = api.fields.find(Drupal.ParagraphsExtra.readData($(this), 'ajax-api', 'field-name'));
        field.instances.clear();

        var is_new = false;
        if (!$(this).data('ParagraphsExtra.Api.FieldInstance')) {
          is_new = true;
        }

        var instance = new Drupal.ParagraphsExtra.Api.FieldInstances.FieldInstance($(this));
        instances.pushKeyVal(Drupal.ParagraphsExtra.readData($(this), 'ajax-api', 'instance-id'), instance);
        field.instances.pushKeyVal(instance.instance_id, instance);

        if (is_new) {
          Drupal.ParagraphsExtra.Api.FieldInstances.listener.trigger(instance, 'create', context);
        }
        else {
          Drupal.ParagraphsExtra.Api.FieldInstances.listener.trigger(instance, 'ajax', context);
        }
        Drupal.ParagraphsExtra.Api.FieldInstances.listener.trigger(instance, 'update', context);
      });
      this.instances = instances;

      this.getFieldInstance = function(instance_id) {
        return this.instances.find(instance_id);
      }

      this.getFieldInstances = function() {
        return this.instances;
      }
    },

    listener: new Drupal.ParagraphsExtra.Api.EventListener({
      create: 'onCreate',
      ajax: 'onAjax',
      update: 'onUpdate',
    }),

    FieldInstance: function($field_instance) {

      this.$ = $field_instance;
      this.instance_id = Drupal.ParagraphsExtra.readData($field_instance, 'ajax-api', 'instance-id');
      this.field = Drupal.ParagraphsExtra.Api.getField(Drupal.ParagraphsExtra.readData($field_instance, 'ajax-api', 'field-name'));
      this.paragraphs = new Drupal.ParagraphsExtra.Api.List();

      this.$.data('ParagraphsExtra.Api.FieldInstance', this);

      var selector =
        '.' + Drupal.ParagraphsExtra.className('ajax-api', 'paragraphs-item') +
        '[' + Drupal.ParagraphsExtra.dataName('ajax-api', 'owner') + '="' + this.instance_id + '"]';
      var instance = this;
      $field_instance.find(selector).each(function() {
        var delta = Drupal.ParagraphsExtra.readData($(this), 'ajax-api', 'delta');
        var paragraph = $(this).data('ParagraphsExtra.Api.Paragraph');
        paragraph.setInstance(instance);
        instance.paragraphs.pushKeyVal(delta, paragraph);
        var e;
        while (e = paragraph.loadEvents.shift()) {
          Drupal.ParagraphsExtra.Api.Paragraphs.listener.trigger(paragraph, e[0], e[1]);
        }
      });

      this.getParagraph = function(delta) {
        return this.paragraphs.find(delta);
      }

      this.getParagraphs = function() {
        return this.paragraphs;
      }

      this.getInstanceSettings = function() {
        var this_instance = null;
        var all_instances = Drupal.ParagraphsExtra.Api.settings.find('instance_settings');
        if (all_instances) {
          if (all_instances.hasOwnProperty(this.instance_id)) {
            this_instance = all_instances[this.instance_id];
          }
        }
        return this_instance;
      }

      this.getInstanceSetting = function(name) {
        var setting_value = null;
        var this_instance = this.getInstanceSettings();
        if (this_instance) {
          if (name in this_instance) {
            setting_value = this_instance[name];
          }
        }
        return setting_value;
      }
    }

  };
  Drupal.behaviors.ParagraphsExtraApi = {
    attach: function (context, settings) {
      var nodeName = $(context).attr('nodeName');

      // Only do a full initialization / load on document load.
      if (nodeName && nodeName.toLowerCase() == '#document') {
        Drupal.ParagraphsExtra.Api.init(context, settings);
        Drupal.ParagraphsExtra.Api.load(context, settings);
      }

      // Refresh the object model any time the DOM is minipulated.
      Drupal.ParagraphsExtra.Api.refresh(context, settings);
    }
  }
}(jQuery));
