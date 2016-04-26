<?php
/**
 * @file
 * API Implementation Examples.
 */

/**
 * Alter the result of an AJAX command before it is sent back to the browser.
 *
 * @param array &$element
 *   The element that was rebuilt.
 * @param array $context
 *   The context array for the rebuilt element.
 */
function hook_paragraphs_extra_ajax_api_result_alter(&$element, $context) {
}

/**
 * Completely override the bundle selection page.
 *
 * @param string $instance_id
 *   The instance id of the field.
 */
function hook_paragraphs_extra_bundle_info_page($instance_id) {
}

/**
 * Alter the results of the bundle selection page.
 *
 * @param array &$output
 *   The render array for the modal content.
 * @param array $instance_id
 *   The instance id of the field.
 */
function hook_paragraphs_extra_bundle_info_page_alter(&$output, $instance_id) {
}

function hook_paragraphs_extra_widget_info() {
  return array(
    'custom_widget' => array(
      'title' => t('My Widget'),
      'weight' => 100,
      'class' => '\Drupal\my_module\ParagraphsExtra\Widget\Plugins\MyWidget',
      'settings callback' => 'my_module_widget_settings',
      'file' => 'my_module.test.inc',
      'file path',
    ),
  );
}

function hook_paragraphs_extra_widget_info_alter(&$widgets) {
}
