<?php

namespace Drupal\paragraphs_extra\Widget;

class Api {

  static function getCurrentWidget($types, $element, &$form_state, $context = array()) {
    $widgets = self::widgets();
    if (isset($element['add_more']['paragraphs_extra']['widget']['active']['value']['#value'])) {
      $active = $element['add_more']['paragraphs_extra']['widget']['active']['value']['#value'];
    }
    else {
      $active = NULL;
    }

    if (!isset($types['list'][$active]) || !$types['list'][$active]['enabled']) {
      $active = NULL;
    }

    if (!$active) {
      if (!empty($types['list'])) {
        foreach ($types['list'] as $type => $info) {
          if ($info['enabled']) {
            $active = $type;
            break;
          }
        }
      }
    }

    if (!$active) {
      throw new \Exception('No suitable widget!');
    }

    $class = $widgets[$active]['class'];
    $context['settings'] = $types['settings'][$active];
    return new $class($active, $element, $form_state, $context);
  }

  static function getWidget($types, $element, &$form_state, $context = array()) {
    $widgets = self::widgets();

    $active = NULL;
    if (isset($form_state['triggering_element'])) {
      $button = $form_state['triggering_element'];
      if (isset($button['#submit'][0]) && $button['#submit'][0] == 'paragraphs_extra_widget_switch_submit') {
        $values = drupal_array_get_nested_value($form_state['input'], array_slice($button['#parents'], 0, -1));
        if (isset($values['value'])) {
          $active = $values['value'];
        }
      }
    }

    if (!$active && isset($element['add_more']['paragraphs_extra']['widget']['active']['#value'])) {
      $active = $element['add_more']['paragraphs_extra']['widget']['active']['#value'];
    }
  

    if (!isset($types['list'][$active]) || !$types['list'][$active]['enabled']) {
      $active = NULL;
    }

    if (!$active) {
      $widget = self::getCurrentWidget($types, $element, $form_state, $context = array());
    }
    else {
      $class = $widgets[$active]['class'];
      $context['settings'] = $types['settings'][$active];
      $widget = new $class($active, $element, $form_state, $context);
    }

    return $widget;
  }

  static public function widgets() {
    $widgets = module_invoke_all('paragraphs_extra_widget_info') + paragraphs_extra_widget_info();
    uasort($widgets, 'drupal_sort_weight');
    drupal_alter('paragraphs_extra_widget_info', $widgets);
    return $widgets;
  }

  static public function widgetInstances($types, &$element, &$form_state, $context) {
    $info = self::widgets();
    $widgets = array();
    foreach ($types['list'] as $name => $settings) {
      if ($settings['enabled']) {
        $context['settings'] = $types['settings'][$name];
        $class = $info[$name]['class'];
        $widgets[$name] = new $class($name, $element, $form_state, $context);
      }
    }
    return $widgets;
  }
}
